# RAG Agent for Documentation Search

## Overview

This project delivers an on-premise **Retrieval-Augmented Generation (RAG) agent** that allows users to query internal documentation in natural language. All components run locally — no data leaves the organisation.

## Repository Structure

```
docs/rag-agent/
├── README.md               ← this file
├── roadmap.md              ← milestones & phase descriptions
├── gantt.md                ← visual GANTT chart (Mermaid)
├── infrastructure.md       ← hardware & software environment
├── phase-0-setup.md        ← Phase 0: Environment Setup
├── phase-1-poc.md          ← Phase 1: Proof of Concept
├── phase-2-mvp.md          ← Phase 2: Minimum Viable Product
└── phase-3-expansion.md    ← Phase 3: Project Expansion
```

## Quick Reference

| Phase | Duration | Goal |
|---|---|---|
| 0 – Environment Setup | 2 weeks | Working local stack (LLM + K8s) |
| 1 – POC | 4 weeks | End-to-end RAG pipeline on one document corpus |
| 2 – MVP | 6 weeks | Production-grade, containerised, with UI |
| 3 – Expansion | 8 weeks | Multi-source, hybrid search, CI/CD, monitoring |

**Total estimated duration: ~20 weeks (5 months)** for a single developer.

## Key Technology Choices

| Component | Technology |
|---|---|
| On-premise LLM runtime | [Ollama](https://ollama.com/) |
| LLM model | Mistral 7B (4-bit GGUF) / Llama 3 8B |
| RAG framework | [LangChain](https://www.langchain.com/) + [LlamaIndex](https://www.llamaindex.ai/) |
| Vector database | [Qdrant](https://qdrant.tech/) |
| Orchestration | Kubernetes (k3s) |
| AI coding assistant | [Cline](https://github.com/cline/cline) |
| UI | Streamlit (POC→MVP) / React (Expansion) |
| Ingestion pipeline | Python, Docling / Unstructured.io |

## Hardware Summary

See [infrastructure.md](infrastructure.md) for full details.

* **CPU** Intel Xeon w-2343 (8 cores / 16 threads)
* **RAM** 32 GB DDR4
* **GPU** Nvidia RTX A2000 12 GB VRAM — sufficient for 7–8 B parameter models at 4-bit quantisation with GPU offloading via `llama.cpp` / Ollama
