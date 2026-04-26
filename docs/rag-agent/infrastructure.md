# Infrastructure & Environment

## Hardware

| Component | Specification |
|---|---|
| CPU | Intel Xeon w-2343 (8 cores / 16 threads, 3.2 GHz base) |
| RAM | 32 GB DDR4 ECC |
| GPU | Nvidia RTX A2000 12 GB GDDR6 (Ampere, 3328 CUDA cores) |
| Storage (recommended) | ≥ 500 GB NVMe SSD (OS + models + vector DB) |
| Network | 1 GbE minimum (10 GbE recommended for multi-user) |

### GPU Capacity Analysis

The RTX A2000 (12 GB VRAM) enables on-premise inference of the following models at 4-bit quantisation (GGUF format via `llama.cpp` / Ollama):

| Model | VRAM Required (Q4_K_M) | Fits? |
|---|---|---|
| Mistral 7B | ~4.5 GB | ✅ |
| Llama 3 8B | ~5.0 GB | ✅ |
| Phi-3 Mini 3.8B | ~2.5 GB | ✅ |
| Llama 3 70B | ~40 GB | ❌ (CPU-only, very slow) |
| Mixtral 8×7B | ~26 GB | ❌ (partial offload possible) |

**Recommended model for this hardware:** `mistral:7b-instruct-q4_K_M` or `llama3:8b-instruct-q4_K_M`.

---

## Operating System

* **Ubuntu 22.04 LTS** (Jammy Jellyfish) — long-term support until April 2027
* Kernel ≥ 5.15 (default in 22.04)

---

## Software Stack

### System-level

| Software | Version | Purpose |
|---|---|---|
| Nvidia Driver | ≥ 525 | GPU access |
| CUDA Toolkit | 12.x | GPU compute |
| Docker Engine | ≥ 24.0 | Container runtime |
| k3s | ≥ 1.29 | Lightweight Kubernetes |
| Helm | ≥ 3.14 | Kubernetes package manager |
| Python | 3.11 | Application runtime |

### Kubernetes Add-ons

| Add-on | Purpose |
|---|---|
| Nvidia GPU Operator | Exposes GPU to pods |
| local-path-provisioner | Provides PersistentVolumeClaims on single-node k3s |
| Prometheus + Grafana (kube-prometheus-stack) | Metrics & dashboards |
| Qdrant Helm chart | Vector database |
| Nginx Ingress Controller | HTTP routing to services |

### Application Stack

| Component | Technology | Notes |
|---|---|---|
| LLM runtime | Ollama | Runs as a K8s Deployment, GPU-enabled |
| Embedding model | `nomic-embed-text` (via Ollama) | 768-dim embeddings |
| Vector database | Qdrant | Persistent volume on NVMe |
| RAG framework | LangChain | Orchestrates retrieval + generation |
| Ingestion | Python + Docling / Unstructured.io | Handles PDF, DOCX, Markdown, HTML |
| API server | FastAPI | REST interface for the frontend |
| UI (MVP) | Streamlit | Rapid prototype |
| UI (Expansion) | React / Next.js | Production-grade frontend |
| AI coding assistant | Cline (VS Code extension) | Points to local Ollama endpoint |

---

## Network Architecture

```
┌──────────────────────────────────────────────────────────┐
│                  Ubuntu Machine (LAN)                    │
│                                                          │
│  ┌────────────────────────────────────────────────────┐  │
│  │                k3s Kubernetes Cluster              │  │
│  │                                                    │  │
│  │  ┌──────────┐  ┌──────────┐  ┌─────────────────┐  │  │
│  │  │  Ollama  │  │  Qdrant  │  │  Ingestion Job  │  │  │
│  │  │ (GPU pod)│  │ (vector) │  │   (CronJob)     │  │  │
│  │  └──────────┘  └──────────┘  └─────────────────┘  │  │
│  │                                                    │  │
│  │  ┌──────────┐  ┌──────────┐  ┌─────────────────┐  │  │
│  │  │ FastAPI  │  │Streamlit │  │  Prometheus +   │  │  │
│  │  │   API   │  │    UI    │  │    Grafana      │  │  │
│  │  └────┬─────┘  └────┬─────┘  └─────────────────┘  │  │
│  │       │             │                              │  │
│  │  ┌────▼─────────────▼──────────────────────────┐  │  │
│  │  │           Nginx Ingress Controller           │  │  │
│  │  └──────────────────┬──────────────────────────┘  │  │
│  └─────────────────────┼──────────────────────────────┘  │
│                        │ :80 / :443                       │
└────────────────────────┼─────────────────────────────────┘
                         │
                    LAN clients
```

---

## Storage Layout

| Path | Contents | Recommended size |
|---|---|---|
| `/var/lib/rancher/k3s` | k3s state | 20 GB |
| `/mnt/rag/models` | Ollama model weights | 20–50 GB |
| `/mnt/rag/qdrant` | Vector DB data | 10–50 GB (scales with corpus) |
| `/mnt/rag/docs-raw` | Original source documents | as needed |
| `/mnt/rag/docs-processed` | Chunked / embedded artefacts | as needed |

---

## Cline Configuration

Cline should be configured to use the local Ollama endpoint so that AI assistance is fully on-premise.

In VS Code settings (`settings.json`):

```json
{
  "cline.apiProvider": "ollama",
  "cline.ollamaBaseUrl": "http://localhost:11434",
  "cline.ollamaModelId": "mistral:7b-instruct-q4_K_M"
}
```

> **Note:** Replace `localhost` with the machine's LAN IP if Cline runs on a different workstation.
