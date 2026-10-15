#pragma once
int classify(float f[]) {
    if (f[19] <= 0.863867998f) {
        if (f[13] <= -0.328116491f) {
            if (f[26] <= 0.120474521f) {
                return 3; // std (samples: 63)
            } else {
                if (f[19] <= 0.121490497f) {
                    if (f[6] <= 0.310984477f) {
                        return 5; // wlk (samples: 58)
                    } else {
                        return 4; // ups (samples: 2)
                    }
                } else {
                    if (f[1] <= -0.404690864f) {
                        return 4; // ups (samples: 38)
                    } else {
                        return 0; // dws (samples: 34)
                    }
                }
            }
        } else {
            if (f[26] <= 0.336115539f) {
                if (f[2] <= 0.142895725f) {
                    if (f[2] <= -0.126780767f) {
                        return 2; // sit (samples: 2)
                    } else {
                        return 0; // dws (samples: 2)
                    }
                } else {
                    return 3; // std (samples: 23)
                }
            } else {
                if (f[19] <= 0.850005984f) {
                    return 2; // sit (samples: 5058)
                } else {
                    if (f[30] <= 1.00567669f) {
                        return 3; // std (samples: 4)
                    } else {
                        return 2; // sit (samples: 38)
                    }
                }
            }
        }
    } else {
        if (f[9] <= 0.451770186f) {
            if (f[26] <= 0.443864897f) {
                if (f[19] <= 1.7044605f) {
                    if (f[17] <= -1.28511846f) {
                        return 4; // ups (samples: 1)
                    } else {
                        if (f[14] <= -0.562209994f) {
                            if (f[20] <= 0.316299494f) {
                                return 4; // ups (samples: 1)
                            } else {
                                return 3; // std (samples: 1)
                            }
                        } else {
                            if (f[30] <= 1.01807278f) {
                                if (f[6] <= 0.148138218f) {
                                    return 3; // std (samples: 4673)
                                } else {
                                    return 3; // std (samples: 3)
                                }
                            } else {
                                if (f[5] <= 0.0377469044f) {
                                    return 0; // dws (samples: 2)
                                } else {
                                    return 3; // std (samples: 4)
                                }
                            }
                        }
                    }
                } else {
                    if (f[23] <= 0.456480995f) {
                        return 4; // ups (samples: 1)
                    } else {
                        return 5; // wlk (samples: 2)
                    }
                }
            } else {
                return 2; // sit (samples: 39)
            }
        } else {
            if (f[7] <= 0.930620402f) {
                if (f[4] <= -0.250802621f) {
                    if (f[4] <= -0.49885951f) {
                        if (f[24] <= 1.07652617f) {
                            if (f[7] <= 0.149211414f) {
                                return 5; // wlk (samples: 2)
                            } else {
                                if (f[4] <= -1.55334914f) {
                                    return 1; // jog (samples: 2)
                                } else {
                                    return 0; // dws (samples: 805)
                                }
                            }
                        } else {
                            return 1; // jog (samples: 4)
                        }
                    } else {
                        if (f[2] <= -0.107791442f) {
                            if (f[3] <= 0.164113037f) {
                                if (f[5] <= 0.224030748f) {
                                    return 0; // dws (samples: 142)
                                } else {
                                    return 0; // dws (samples: 13)
                                }
                            } else {
                                if (f[12] <= -0.929771483f) {
                                    return 0; // dws (samples: 12)
                                } else {
                                    return 4; // ups (samples: 17)
                                }
                            }
                        } else {
                            if (f[2] <= 0.169570804f) {
                                if (f[23] <= 2.42149448f) {
                                    return 5; // wlk (samples: 92)
                                } else {
                                    return 0; // dws (samples: 35)
                                }
                            } else {
                                if (f[14] <= -0.209457502f) {
                                    return 0; // dws (samples: 38)
                                } else {
                                    return 2; // sit (samples: 2)
                                }
                            }
                        }
                    }
                } else {
                    if (f[4] <= 0.330258772f) {
                        if (f[2] <= -0.152392156f) {
                            if (f[31] <= 0.368293688f) {
                                if (f[14] <= -0.927521497f) {
                                    return 4; // ups (samples: 266)
                                } else {
                                    return 4; // ups (samples: 95)
                                }
                            } else {
                                if (f[5] <= 0.077793818f) {
                                    return 0; // dws (samples: 774)
                                } else {
                                    return 5; // wlk (samples: 391)
                                }
                            }
                        } else {
                            if (f[8] <= 0.212223336f) {
                                if (f[20] <= 0.368804991f) {
                                    return 4; // ups (samples: 280)
                                } else {
                                    return 4; // ups (samples: 173)
                                }
                            } else {
                                if (f[2] <= 0.183033042f) {
                                    return 5; // wlk (samples: 5175)
                                } else {
                                    return 4; // ups (samples: 266)
                                }
                            }
                        }
                    } else {
                        if (f[4] <= 0.486417234f) {
                            if (f[1] <= 0.986877054f) {
                                if (f[2] <= -0.116444506f) {
                                    return 4; // ups (samples: 90)
                                } else {
                                    return 4; // ups (samples: 77)
                                }
                            } else {
                                if (f[20] <= 0.421958998f) {
                                    return 4; // ups (samples: 25)
                                } else {
                                    return 5; // wlk (samples: 71)
                                }
                            }
                        } else {
                            if (f[31] <= 0.623690754f) {
                                if (f[4] <= 0.836153924f) {
                                    return 4; // ups (samples: 374)
                                } else {
                                    return 4; // ups (samples: 460)
                                }
                            } else {
                                if (f[11] <= 1.49044818f) {
                                    return 1; // jog (samples: 30)
                                } else {
                                    return 4; // ups (samples: 48)
                                }
                            }
                        }
                    }
                }
            } else {
                if (f[13] <= -0.327079996f) {
                    if (f[12] <= -0.230514996f) {
                        if (f[2] <= 0.252085872f) {
                            if (f[3] <= -0.562115729f) {
                                if (f[25] <= 1.46592206f) {
                                    return 4; // ups (samples: 4)
                                } else {
                                    return 1; // jog (samples: 2)
                                }
                            } else {
                                if (f[13] <= -0.549384505f) {
                                    return 1; // jog (samples: 1937)
                                } else {
                                    return 1; // jog (samples: 42)
                                }
                            }
                        } else {
                            if (f[29] <= 1.05972949f) {
                                return 0; // dws (samples: 1)
                            } else {
                                return 4; // ups (samples: 3)
                            }
                        }
                    } else {
                        if (f[23] <= 4.26412058f) {
                            return 5; // wlk (samples: 9)
                        } else {
                            return 1; // jog (samples: 4)
                        }
                    }
                } else {
                    if (f[9] <= 1.70161563f) {
                        if (f[8] <= 0.3654899f) {
                            return 1; // jog (samples: 1)
                        } else {
                            return 5; // wlk (samples: 7)
                        }
                    } else {
                        return 0; // dws (samples: 9)
                    }
                }
            }
        }
    }
}