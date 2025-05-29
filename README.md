# FT-TL: FakeTime Tensor Language

**Drop-in, ultra-performant model backend for quantized GGUF inference on modern CPUs.**

FT-TL is a no-compromise execution engine designed for **low-level precision, thread-scaled throughput, and ruthless efficiency**. Built from scratch in modern C++23, it has been carefully engineered to **obliterate** most, if not all, competing inference libraries, 
 especially on **modern, high-performance hardware**.

---

## 🚀 Example Usage

```cpp
int32_t main(int argc, const char**argv) {
    static constexpr rt_tm::global_config global_config{ .exceptions = true };

    rt_tm::model_graph model_graph = rt_tm::core<global_config>::parse_model(argv[2]);

    rt_tm::op_graph_config graph_config{ .num_threads = 12 };
    rt_tm::op_graph op_graph{ rt_tm::core<global_config>::create_graph(graph_config, model_graph) };

    rt_tm::input_session_config session_config{ std::cin, .max_length = 1024 };
    rt_tm::input_session input_session{ session_config };

    while (input_session) {
        op_graph.process_input(input_session);
    }
    return 0;
}
```

> This is not a toy. This is a **streamed execution ritual** that:
> - Parses the GGUF model directly with compile-time config fidelity
> - Constructs a low-level op graph with static execution specialization
> - Defines streaming input parameters declaratively
> - Executes optimized kernels across threads with zero boilerplate

If it looks simple, it's because **everything complicated is already fused, tiled, and dispatched under the hood.**

---

## 🧾 Why FT-TL?

Other backends work *within* the constraints of GGML’s original design.  
FT-TL was built from the ground up to **obliterate them.**

Where others abstract, we expose.  
Where others patch, we re-architect.  
Where others add options, we **eliminate friction.**

| Feature                        | FT-TL                                | GGML / llama.cpp               |
|-------------------------------|--------------------------------------|-------------------------------|
| Graph-level execution model   | True op graph                        | Flat static pass              |
| Threading                     | Configurable, fused, stream-aware    | Loop-threaded with patchwork  |
| Input interface               | Streamed, session-managed            | Token-by-token state manual   |
| Backend extensibility         | Kernel registry + dispatch engine    | Templated monolith            |
| Design origin                 | Performance-first, no legacy         | Evolved from debug engine     |
| SIMD tiling                   | Planned from day 0                   | Retroactive optimization      |

FT-TL isn't trying to fix what's broken —  
**it's rewriting the baseline for what a CPU backend *should be*.**

---

## 🧠 Philosophy
FT-TL is built on the belief that:

- **Performance comes from architectural clarity** — not layers of abstraction
- **Throughput is king** — and only measurable in tokens per second
- **The graph is sacred** — parsing is understanding
- **If it doesn’t outperform llama.cpp, it’s not done**

---

## ✨ Features
- 🔥 SIMD-optimized op kernels (AVX2/AVX-512/NEON)
- 🔁 Streamed input support (interactive or batch)
- 🧩 Modular graph parsing from raw GGUF
- 🧵 Scalable thread model with minimal overhead
- 💥 Zero boost, zero std::shared_ptr, zero bloat

---

## 📚 Documentation

Explore FT-TL’s internal structure and philosophy:

* [🧠 Core_Api.md – Entry-point API for model parsing and execution graph creation](Documentation/Core_Api.md)
* [🧱 Core.md – Naming system and architectural layers](Documentation/Core.md)

---

## 💬 License
MIT. No patents. Use it. Break it. Ship it.
