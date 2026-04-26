# Phase 2 – Minimum Viable Product (MVP)

**Duration:** 6 weeks (Weeks 7–12)  
**Owner:** Developer  
**Prerequisite:** POC acceptance criteria met; go/no-go decision is "go"  
**Goal:** A production-ready, containerised system deployed on Kubernetes with a browser UI, accessible to a pilot group of users.

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     k3s Kubernetes Cluster                      │
│                                                                 │
│  ┌─────────────────┐   ┌──────────────┐   ┌─────────────────┐  │
│  │  Ollama          │   │   Qdrant     │   │  Ingestion      │  │
│  │  Deployment      │   │  StatefulSet │   │  CronJob        │  │
│  │  (GPU: 1)        │   │  PVC: 20 GB  │   │  (nightly)      │  │
│  └────────┬─────────┘   └──────┬───────┘   └─────────────────┘  │
│           │                   │                                  │
│  ┌────────▼───────────────────▼────────────────────────────┐    │
│  │                     FastAPI Service                      │    │
│  │              /query   /health   /docs (OpenAPI)          │    │
│  └────────────────────────┬────────────────────────────────┘    │
│                           │                                      │
│  ┌────────────────────────▼────────────────────────────────┐    │
│  │                   Streamlit UI Service                   │    │
│  └────────────────────────┬────────────────────────────────┘    │
│                           │                                      │
│  ┌────────────────────────▼────────────────────────────────┐    │
│  │              Nginx Ingress Controller                    │    │
│  │    rag.local → Streamlit   /api → FastAPI               │    │
│  └──────────────────────────────────────────────────────── ┘    │
│                                                                  │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │           Prometheus + Grafana (kube-prometheus-stack)    │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

---

## Task List

### 2.1 — Containerise Components

Create a `Dockerfile` for each service (ingestion, API, UI). Use a shared `requirements.txt`.

**Directory layout:**

```
rag-agent/
├── ingestion/
│   ├── Dockerfile
│   ├── ingest.py
│   └── requirements.txt
├── api/
│   ├── Dockerfile
│   ├── main.py          # FastAPI application
│   └── requirements.txt
├── ui/
│   ├── Dockerfile
│   ├── app.py           # Streamlit application
│   └── requirements.txt
└── helm/
    └── rag-agent/       # Helm chart
```

Push images to a local registry:

```bash
# Start a local OCI registry
docker run -d -p 5000:5000 --name registry registry:2

# Build & push
docker build -t localhost:5000/rag-ingestion:latest ./ingestion
docker push localhost:5000/rag-ingestion:latest
# ... repeat for api, ui
```

### 2.2 — Helm Chart

Create a Helm chart `helm/rag-agent` with the following templates:

| Template | Resource |
|---|---|
| `ollama.yaml` | Deployment + Service (GPU limit: 1) |
| `qdrant.yaml` | StatefulSet + PVC + Service |
| `api.yaml` | Deployment + Service + HPA |
| `ui.yaml` | Deployment + Service |
| `ingestion-cronjob.yaml` | CronJob (schedule: `0 2 * * *`) |
| `ingress.yaml` | Nginx Ingress rules |
| `serviceaccount.yaml` | RBAC for the ingestion job |

Deploy:

```bash
helm upgrade --install rag-agent ./helm/rag-agent \
  --namespace rag \
  --create-namespace \
  -f values.yaml
```

### 2.3 — Nightly Ingestion CronJob

The CronJob:

1. Pulls fresh documents from the source (NFS mount or git clone).
2. Detects new/modified files (hash-based delta).
3. Chunks, embeds, and upserts into Qdrant.
4. Logs a summary to stdout (captured by Kubernetes).

### 2.4 — FastAPI REST API

Key endpoints:

```
POST /query
  Body:  { "question": "string", "top_k": 5 }
  Returns: { "answer": "string", "sources": [...], "latency_ms": 1234 }

GET  /health      → 200 OK
GET  /docs        → Swagger UI (OpenAPI 3.0)
```

Enable CORS for the Streamlit UI origin.

### 2.5 — Streamlit UI

Features:

- Text input for the question.
- Answer displayed with source citations (clickable doc name + chunk preview).
- Sidebar: corpus statistics (document count, last indexed timestamp).
- Response time indicator.

### 2.6 — Authentication & Access Control

Use **Basic Auth** at the Nginx Ingress level for MVP:

```bash
htpasswd -c auth admin
kubectl create secret generic basic-auth --from-file=auth -n rag
```

Annotate the Ingress:

```yaml
annotations:
  nginx.ingress.kubernetes.io/auth-type: basic
  nginx.ingress.kubernetes.io/auth-secret: basic-auth
```

### 2.7 — Observability

Install `kube-prometheus-stack`:

```bash
helm repo add prometheus-community https://prometheus-community.github.io/helm-charts
helm install kube-prom prometheus-community/kube-prometheus-stack \
  --namespace monitoring --create-namespace
```

Instrument FastAPI with `prometheus-fastapi-instrumentator`.

Key Grafana dashboards to create:

| Dashboard | Panels |
|---|---|
| RAG Overview | Requests/min, P50/P95 latency, error rate |
| GPU Utilisation | GPU util %, VRAM used, GPU temperature |
| Kubernetes | Pod restarts, CPU, memory per namespace |

### 2.8 — User Acceptance Testing

- Recruit 3–5 pilot users from the team that owns the pilot corpus.
- Run for one week.
- Collect feedback via a short Google Form (or equivalent on-prem tool).
- Track: answer quality (1–5 stars), missing topics, UI issues.

---

## Acceptance Criteria

| Criterion | Target |
|---|---|
| Full stack deploys with `helm upgrade --install` in < 5 minutes | ✅ |
| All pods `Running` / `Completed` within 5 minutes of deploy | ✅ |
| POST /query returns answer with sources in < 10 s (P95) | ✅ |
| System handles 5 concurrent users without degradation | ✅ |
| Nightly CronJob runs without errors for 5 consecutive nights | ✅ |
| Grafana dashboard shows GPU and latency metrics | ✅ |
| UAT: ≥ 3 of 5 users rate answer quality ≥ 3/5 | ✅ |

---

## Notes

- Set Kubernetes resource requests/limits carefully. Ollama needs `nvidia.com/gpu: 1` in its resource spec.
- Use `readinessProbe` on the Ollama pod targeting `GET /api/tags` — model loading takes ~30 s on first boot.
- Store the `htpasswd` file in a Kubernetes Secret, never in the Helm chart values committed to git.
