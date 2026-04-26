# Phase 3 – Project Expansion

**Duration:** 8 weeks (Weeks 13–20)  
**Owner:** Developer  
**Prerequisite:** MVP live and UAT passed  
**Goal:** Broaden the system to cover the whole organisation, improve retrieval quality, and reach operational maturity with CI/CD, monitoring, and runbooks.

---

## Task List

### 3.1 — Multi-Source Ingestion

Extend the ingestion pipeline to support additional document sources:

| Source | Connector |
|---|---|
| Confluence | REST API (Confluence Cloud or Server) |
| GitLab / GitHub Wikis | Git clone + Markdown loader |
| Local file share (SMB/NFS) | Mount as Kubernetes PV |
| Google Drive (optional) | Google Drive API + service account |
| Jira (optional) | REST API for issue descriptions |

Create a pluggable `SourceConnector` interface:

```python
class SourceConnector(ABC):
    @abstractmethod
    def list_documents(self) -> list[DocumentRef]: ...

    @abstractmethod
    def fetch(self, ref: DocumentRef) -> RawDocument: ...
```

Connectors are selected by configuration (YAML) and run within the nightly CronJob.

### 3.2 — Hybrid Search (BM25 + Dense)

Replace pure vector search with **Reciprocal Rank Fusion** of:

1. **Dense retrieval** — existing `nomic-embed-text` embeddings in Qdrant.
2. **Sparse retrieval** — BM25 via Qdrant's native sparse vector support (or a lightweight Elasticsearch alternative).

```python
from langchain.retrievers import EnsembleRetriever
from langchain_community.retrievers import BM25Retriever

dense_retriever = vectorstore.as_retriever(search_kwargs={"k": 10})
bm25_retriever  = BM25Retriever.from_documents(chunks, k=10)

ensemble = EnsembleRetriever(
    retrievers=[bm25_retriever, dense_retriever],
    weights=[0.4, 0.6],
)
```

Re-run the gold-set evaluation to confirm improvement.

### 3.3 — Feedback Loop

Add a thumbs-up / thumbs-down button to the UI.  
Store feedback in a PostgreSQL table (or SQLite for simplicity):

```sql
CREATE TABLE feedback (
    id          SERIAL PRIMARY KEY,
    question    TEXT,
    answer      TEXT,
    sources     JSONB,
    rating      SMALLINT,   -- 1 = positive, -1 = negative
    comment     TEXT,
    created_at  TIMESTAMPTZ DEFAULT now()
);
```

Weekly task: Review negative ratings, identify root causes (bad chunks, wrong source, hallucination), and add failing cases to the gold-set.

### 3.4 — CI/CD Pipeline

Use **GitLab CI** (or GitHub Actions if the repo is on GitHub):

```yaml
# .gitlab-ci.yml (outline)
stages:
  - lint
  - test
  - build
  - deploy

lint:
  stage: lint
  script: [ruff check ., mypy .]

test:
  stage: test
  script: [pytest tests/ -v --cov=src]

build:
  stage: build
  script:
    - docker build -t $CI_REGISTRY_IMAGE/api:$CI_COMMIT_SHA ./api
    - docker push $CI_REGISTRY_IMAGE/api:$CI_COMMIT_SHA

deploy:
  stage: deploy
  script:
    - helm upgrade --install rag-agent ./helm/rag-agent
        --set api.image.tag=$CI_COMMIT_SHA
  environment: production
  only: [main]
```

Pipeline gates:

- All lint checks pass.
- All unit tests pass (coverage ≥ 80 %).
- Docker build succeeds.
- Helm dry-run passes.

### 3.5 — Advanced Chunking

Replace fixed-size chunking with:

1. **Semantic chunking** — split at natural topic boundaries using embedding cosine similarity drops.
2. **Table extraction** — `docling` or `unstructured` table-aware parsing.
3. **Image extraction** (optional) — extract figures with captions as separate chunks.

```python
from langchain_experimental.text_splitter import SemanticChunker

splitter = SemanticChunker(
    embeddings=OllamaEmbeddings(model="nomic-embed-text"),
    breakpoint_threshold_type="percentile",
)
chunks = splitter.split_documents(docs)
```

Re-run RAGAS evaluation after switching.

### 3.6 — Model Fine-Tuning (Optional)

Evaluate whether domain-adapted embeddings improve retrieval quality:

1. Generate a synthetic training dataset using the LLM (question → positive chunk pairs).
2. Fine-tune `nomic-embed-text` or `bge-small-en` using `sentence-transformers`.
3. A/B test against the baseline on the gold-set.

> ⚠️ This task is marked optional. Proceed only if the gold-set score plateaus below the 0.85 faithfulness target after steps 3.1–3.5.

### 3.7 — React / Next.js UI

Replace Streamlit with a production-quality frontend:

Key features:

- **Persistent chat history** (stored in browser local storage or a backend DB).
- **Document preview pane** — show the source document alongside the answer.
- **Admin panel** — manage corpus, trigger manual re-index, view feedback stats.
- **Responsive design** — usable on mobile.

Stack: Next.js 14 + TypeScript + Tailwind CSS + ShadCN UI components.

Deploy as a Kubernetes Deployment in the `rag` namespace, served via the existing Nginx Ingress.

### 3.8 — Runbook & Operational Documentation

Create `docs/rag-agent/runbook.md` covering:

| Section | Contents |
|---|---|
| Architecture diagram | Up-to-date post-expansion diagram |
| Deployment | Step-by-step `helm upgrade` instructions |
| Model updates | How to pull and switch an Ollama model |
| Backup & restore | Qdrant snapshot commands; restore procedure |
| Scaling | How to add CPU/memory limits; GPU sharing notes |
| Incident response | Common errors → root cause → fix table |
| On-call contacts | Who to call if the system is down |

### 3.9 — Post-Launch Review

- Re-run the gold-set evaluation and compare to Phase 1 baseline.
- Review user feedback statistics (positive rate, most common failure categories).
- Produce a one-page summary: achievements vs. KPIs, open issues, next-quarter backlog.

---

## Acceptance Criteria

| Criterion | Target |
|---|---|
| At least 3 additional document sources ingested | ✅ |
| Hybrid search RAGAS answer relevancy | ≥ 0.80 |
| RAGAS faithfulness | ≥ 0.85 |
| CI/CD pipeline green on merge to main | ✅ |
| Feedback collected and triaged for ≥ 2 weeks | ✅ |
| React UI deployed and used by ≥ 5 users | ✅ |
| Runbook reviewed by at least one other person | ✅ |
| Post-launch review document written | ✅ |

---

## Potential Future Work (Post Phase 3)

| Item | Description |
|---|---|
| Multi-tenant namespaces | Separate Qdrant collections per team with role-based access |
| Streaming responses | Server-Sent Events for token-by-token streaming in the UI |
| LLM gateway | Litellm proxy to allow model switching without code changes |
| Automated regression testing | Nightly RAGAS evaluation run as a CI job |
| Larger model upgrade | Evaluate Llama 3 70B on CPU+GPU offload when more hardware is available |
| Voice interface | Whisper STT → RAG pipeline → TTS response |
