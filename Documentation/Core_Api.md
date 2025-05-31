# 🧠 FT-TL Core API

This document outlines the **entry-point interface** for working with parsed model graphs and constructing runtime execution graphs in FT-TL. The `harbinger` template defines the foundational operations for converting static model data into a ready-to-run graph representation.

---

## 🧱 API Surface

```cpp
template<global_config config>
struct harbinger {
    static model_graph parse_model_graph(std::string_view path);
    static op_graph<config> create_op_graph(op_graph_config, const model_graph&);
};
```

---

### ✨ `parse_model_graph(path)`

Loads a GGUF model file.

* Returns a deserialized `model_graph`
* No memory is allocated — this is a purely structural pass
* Metadata-only: ops, tensor declarations, shape info

**Think of it as:**

> “The graph’s bones — unbound, untouched, unexecuted.”

---

### ⚙️ `create_op_graph(config, model)`

Takes a `model_graph` and builds an `op_graph<config>`.

* Allocates memory for runtime tensor pools
* Resolves op implementations based on:

  * tensor types
  * platform features (e.g., AVX-512, NEON)
* Statically fuses and prepares the graph for threaded execution

**Think of it as:**

> “Igniting the raw blueprint into a hot execution harbinger.”

---

## 🔎 Summary Table

| Function                         | Description                                                 |
| -------------------------------- | ----------------------------------------------------------- |
| `parse_model_graph(path)`        | Parses GGUF into a `model_graph`                            |
| `create_op_graph(config, model)` | Turns the static model into an optimized `op_graph<config>` |

---

## 📚 See Also

* [Core Architecture Overview](./Core.md)
