# 🧠 FT-TL Core Architecture

This document outlines the terminology and architectural philosophy behind the `core` hierarchy in **FT-TL**, our universal tensor backend. These names are more than stylistic choices — they reflect distinct **phases** in the tensor execution lifecycle and serve as a foundational abstraction for device scheduling, memory layout, and optimization.

---

## 🌱 `rt_tm::model_core`

**Represents:**
The raw, deserialized operations (nodes) from a model graph.

**Purpose:**

* Parsed directly from the model file (e.g., GGUF, ONNX, etc.)
* Unscheduled and untyped at the C++ level
* Holds metadata like op type, inputs, outputs, attributes

**Think of it as:**

> “The genome of a neural graph before it’s expressed into hardware.”

**Stored as:**

```cpp
std::vector<model_core>
```

---

## 🔧 `rt_tm::op_core`

**Represents:**
A transformed, validated, typed operation that is ready for execution planning.

**Purpose:**

* Result of compile-time or runtime graph preparation
* Input and output tensor types resolved
* Memory arenas assigned
* Device-neutral execution descriptor

**Think of it as:**

> “A ready-to-launch instruction that just needs a CPU or GPU to land on.”

**Built from:**

```cpp
model_core -> op_core
```

---

## 🧩 `rt_tm::cpu_op_core`

**Represents:**
A `op_core` specialized for CPU execution.

**Purpose:**

* Binds CPU-specific memory pools
* Can dispatch into AVX2, AVX-512, NEON, etc.
* Contains platform-specific execution metadata (tiling, prefetch hints, etc.)

**Think of it as:**

> “An op\_core hardened for x86/ARM assault.”

**Constructed via:**

```cpp
op_core -> cpu_op_core
```

---

## 🔥 `rt_tm::gpu_op_core`

**Represents:**
A `op_core` mapped to GPU execution targets.

**Purpose:**

* CUDA, Vulkan, or Metal-specific dispatch representation
* Tensors already migrated to GPU memory
* Execution tuned for warp/block/thread layout

**Think of it as:**

> “An op\_core that’s strapped to a rocket.”

**Constructed via:**

```cpp
op_core -> gpu_op_core
```

---

## 🧬 Evolution Flow

```plaintext
model_core
   ↓ deserialization & validation
op_core
   ↓ target binding
cpu_op_core / gpu_op_core
   ↓ device dispatch
execution
```

---

## 🔒 Why “core”?

* **Atomic**: Each `core` represents the indivisible logic of an operation.
* **Minimal**: No fluff. Each layer only carries what it *needs*.
* **Composable**: You can fuse, schedule, or parallelize them efficiently.
* **Semantic**: Clean mental separation between stages of execution.

---

## 💡 Summary

The `core` system gives FT-TL:

* Clear delineation of roles across compile/runtime
* A path for constexpr execution in the future
* Perfect-fit abstraction for both CPU and GPU backends

This isn’t just naming.
It’s **an ontology of inference.**

---
