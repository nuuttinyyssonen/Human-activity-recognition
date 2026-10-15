import numpy as np

def export_tree_to_cpp(clf, feature_names=None):
    tree = clf.tree_

    def recurse(node, depth):
        indent = "    " * depth

        if tree.feature[node] == -2:
            class_idx = int(np.argmax(tree.value[node][0]))
            class_name = clf.classes_[class_idx]
            return [f"{indent}return {class_idx}; // {class_name} (samples: {int(tree.n_node_samples[node])})"]

        feat_idx = tree.feature[node]
        threshold = tree.threshold[node]
        feat_name = f"f[{feat_idx}]" if feature_names is None else feature_names[feat_idx]

        result = []
        result.append(f"{indent}if ({feat_name} <= {threshold:.9g}f) {{")
        result.extend(recurse(tree.children_left[node], depth + 1))
        result.append(f"{indent}}} else {{")
        result.extend(recurse(tree.children_right[node], depth + 1))
        result.append(f"{indent}}}")
        return result

    cpp_code = [
        "int classify(float f[]) {",
        *recurse(0, 1),
        "}"
    ]
    return "\n".join(cpp_code)