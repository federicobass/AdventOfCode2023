# Roadmap – RAG Agent for Documentation Search

## Vision

Enable any team member to ask questions about internal documentation and receive accurate, cited answers in seconds — with zero dependency on external cloud services.

---

## Phases & Milestones

### Phase 0 — Environment Setup *(Weeks 1–2)*

**Goal:** A stable, reproducible local platform on which all subsequent work can build.

| # | Milestone | Done when… |
|---|---|---|
| 0.1 | Ubuntu 22.04 LTS configured | OS installed, drivers updated, SSH access verified |
| 0.2 | Nvidia driver + CUDA | `nvidia-smi` reports RTX A2000 correctly |
| 0.3 | k3s (lightweight Kubernetes) running | `kubectl get nodes` shows node `Ready` |
| 0.4 | Ollama serving a base LLM | `ollama run mistral` returns a response |
| 0.5 | Cline integrated with VS Code | Cline can call the local Ollama endpoint |
| 0.6 | Helm & persistent storage configured | A test PVC binds successfully |

**Exit criteria:** Developer can chat with the on-premise LLM from VS Code via Cline.

---

### Phase 1 — Proof of Concept (POC) *(Weeks 3–6)*

**Goal:** Demonstrate end-to-end RAG on a small, controlled document corpus. No production concerns yet — validate the approach and measure quality.

| # | Milestone | Done when… |
|---|---|---|
| 1.1 | Document ingestion pipeline | Markdown / PDF files chunked & embedded into Qdrant |
| 1.2 | Basic retrieval | Top-k semantic search returns relevant chunks |
| 1.3 | LLM answer generation | Full RAG chain: query → retrieve → generate → cite |
| 1.4 | CLI demo | Developer can query corpus from the terminal |
| 1.5 | Evaluation baseline | RAGAS metrics (faithfulness, answer relevancy) recorded |
| 1.6 | POC retrospective | Findings documented; go/no-go decision for MVP |

**Exit criteria:** ≥70 % answer-relevancy score on a 20-question gold-set derived from the pilot corpus.

---

### Phase 2 — Minimum Viable Product (MVP) *(Weeks 7–12)*

**Goal:** A reliable, containerised service deployed on Kubernetes with a simple web UI accessible to other users.

| # | Milestone | Done when… |
|---|---|---|
| 2.1 | Containerise all components | Docker images for ingestion, API, UI pushed to local registry |
| 2.2 | Kubernetes manifests | Helm chart deploys full stack; rollout is repeatable |
| 2.3 | Ingestion job | CronJob re-indexes docs nightly; delta updates supported |
| 2.4 | REST API | `/query` endpoint documented with OpenAPI spec |
| 2.5 | Streamlit UI | Users can search docs via browser; sources are shown |
| 2.6 | Basic auth & access control | Users log in; admin can manage the corpus |
| 2.7 | Observability (basic) | Prometheus + Grafana scrape latency, GPU util, error rate |
| 2.8 | User acceptance testing | Three internal users validate the system for one week |

**Exit criteria:** System serves 5 concurrent users with P95 response < 10 s; uptime ≥ 99 % over UAT week.

---

### Phase 3 — Project Expansion *(Weeks 13–20)*

**Goal:** Harden the system for broader adoption, improve retrieval quality, and introduce operational excellence.

| # | Milestone | Done when… |
|---|---|---|
| 3.1 | Multi-source ingestion | Confluence, local file-share, GitLab wikis all indexed |
| 3.2 | Hybrid search | BM25 + dense vector re-ranking (Reciprocal Rank Fusion) |
| 3.3 | Feedback loop | 👍/👎 collected; weekly drift report generated |
| 3.4 | CI/CD pipeline | GitHub Actions / GitLab CI builds, tests, and deploys on merge |
| 3.5 | Advanced chunking | Semantic chunking replaces fixed-size; table & image extraction |
| 3.6 | Model fine-tuning (optional) | Domain-adapted embeddings evaluated vs. general embeddings |
| 3.7 | React/Next.js UI | Production-quality UI with chat history and document preview |
| 3.8 | Runbook & on-call docs | Incident response, backup/restore, upgrade procedures documented |
| 3.9 | Post-launch review | KPIs reviewed vs. baseline; next-quarter backlog defined |

**Exit criteria:** Answer-relevancy ≥ 80 % on updated gold-set; full CI/CD green; runbook reviewed by a peer.

---

## Risk Register

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| GPU VRAM insufficient for chosen model | Medium | High | Use 4-bit GGUF, enable CPU offload layers in Ollama |
| Document quality is poor (no metadata, inconsistent format) | High | Medium | Pre-ingestion normalisation step; flag low-quality docs |
| k3s networking issues on single-node | Low | Medium | Fall back to `docker compose` for development, k3s for staging |
| Single developer bottleneck / sick leave | High | High | Weekly written status updates; design documented so work can resume easily |
| Model hallucinations on technical docs | Medium | High | Enforce source-grounding in prompt; display citations always |

---

## KPIs

| KPI | POC target | MVP target | Expansion target |
|---|---|---|---|
| Answer relevancy (RAGAS) | ≥ 0.70 | ≥ 0.75 | ≥ 0.80 |
| Faithfulness (RAGAS) | ≥ 0.75 | ≥ 0.80 | ≥ 0.85 |
| P95 end-to-end latency | < 30 s | < 10 s | < 6 s |
| Corpus coverage | 1 team's docs | All internal docs | + external sources |
| Active users | 1 (developer) | 3–5 (pilot team) | Whole organisation |
