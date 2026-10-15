import numpy as np

from load_data import build_dataframe
from windowing import create_windows
from features import extract_features
from export_model import export_tree_to_cpp
from test_cases import write_test_cases, write_replay

from sklearn.model_selection import GroupShuffleSplit, GroupKFold, cross_validate
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import classification_report, confusion_matrix

DEPTHS = [2, 3, 4, 6, 8, 12, None]


def split_by_subject(X, y, subjects, train_size=0.8, seed=42):
    gss = GroupShuffleSplit(n_splits=1, train_size=train_size, random_state=seed)
    train_idx, test_idx = next(gss.split(X, y, groups=subjects))
    return train_idx, test_idx


def depth_sweep(F_train, y_train, train_subjects, depths=DEPTHS):
    n_splits = min(5, len(np.unique(train_subjects)))
    gkf = GroupKFold(n_splits=n_splits)

    train_scores, val_scores = [], []
    print(f"{'Depth':<8} | {'Train F1 (Macro)':<18} | {'Val F1 (Macro)':<18}")
    print("-" * 50)

    for depth in depths:
        clf = DecisionTreeClassifier(max_depth=depth, random_state=42)
        cv = cross_validate(
            clf, F_train, y_train,
            groups=train_subjects,
            cv=gkf,
            scoring="f1_macro",
            return_train_score=True,
        )
        train_scores.append(np.mean(cv["train_score"]))
        val_scores.append(np.mean(cv["test_score"]))
        print(f"{str(depth):<8} | {train_scores[-1]:.4f}             | {val_scores[-1]:.4f}")

    return train_scores, val_scores


def final_evaluation(F_train, y_train, F_test, y_test, depth):
    clf = DecisionTreeClassifier(max_depth=depth, random_state=42)
    clf.fit(F_train, y_train)
    y_pred = clf.predict(F_test)

    print("\n--- Final Test Set Evaluation ---")
    print("Classes:", clf.classes_)
    print(confusion_matrix(y_test, y_pred, labels=clf.classes_))
    print()
    print(classification_report(y_test, y_pred))
    return clf


if __name__ == "__main__":
    df = build_dataframe("../data")
    X, y, subjects = create_windows(df, 100, 50)

    train_idx, test_idx = split_by_subject(X, y, subjects)
    X_train, y_train = X[train_idx], y[train_idx]
    X_test, y_test = X[test_idx], y[test_idx]
    train_subjects = subjects[train_idx]

    F_train = extract_features(X_train)
    F_test = extract_features(X_test)

    print(F_train.shape, F_test.shape)
    print("Train subjects:", np.unique(train_subjects))
    print("Test subjects: ", np.unique(subjects[test_idx]))

    train_scores, val_scores = depth_sweep(F_train, y_train, train_subjects)

    # Selection rule: highest validation score. Consider changing this to the
    # smallest depth within ~0.01 of the best, and justify the choice.
    best_depth = DEPTHS[int(np.argmax(val_scores))]
    print(f"\n-> Selected max_depth: {best_depth}")

    clf = final_evaluation(F_train, y_train, F_test, y_test, best_depth)

    test_subjects = np.unique(subjects[test_idx])
    write_replay(clf, df, test_subjects, "../firmware/replay.h",
                 seg_len=150, per_class=1)

    write_test_cases(clf, X_test, y_test, F_test, "../firmware/test_cases.h")

    # Export tree
    clf = final_evaluation(F_train, y_train, F_test, y_test, best_depth)
    with open("../firmware/tree.h", "w") as fh:
        fh.write("#pragma once\n" + export_tree_to_cpp(clf))