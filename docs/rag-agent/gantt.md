# GANTT Chart – RAG Agent Project

The chart below uses [Mermaid](https://mermaid.js.org/) syntax and renders automatically on GitHub.

```mermaid
gantt
    title RAG Agent for Documentation Search
    dateFormat  YYYY-MM-DD
    axisFormat  %d %b

    section Phase 0 – Setup
    OS & drivers                    :done,    p0_os,   2026-05-04, 3d
    k3s Kubernetes                  :done,    p0_k8s,  2026-05-07, 3d
    Ollama + base LLM               :done,    p0_llm,  2026-05-07, 3d
    Cline + VS Code integration     :done,    p0_cline,2026-05-12, 2d
    Helm + storage                  :done,    p0_helm, 2026-05-12, 2d
    Phase 0 review                  :milestone, p0_done, 2026-05-15, 0d

    section Phase 1 – POC
    Document ingestion pipeline     :active,  p1_ingest, 2026-05-18, 5d
    Qdrant vector DB setup          :         p1_vdb,    2026-05-18, 3d
    Retrieval chain                 :         p1_ret,    2026-05-25, 4d
    LLM answer generation           :         p1_gen,    2026-05-25, 5d
    CLI demo                        :         p1_cli,    2026-06-01, 3d
    RAGAS evaluation baseline       :         p1_eval,   2026-06-01, 3d
    POC retrospective               :milestone, p1_done, 2026-06-12, 0d

    section Phase 2 – MVP
    Dockerise components            :         p2_docker, 2026-06-15, 5d
    Kubernetes / Helm chart         :         p2_helm,   2026-06-22, 5d
    Nightly ingestion CronJob       :         p2_cron,   2026-06-22, 4d
    REST API + OpenAPI spec         :         p2_api,    2026-06-29, 5d
    Streamlit UI                    :         p2_ui,     2026-07-06, 6d
    Auth & access control           :         p2_auth,   2026-07-06, 5d
    Prometheus + Grafana            :         p2_obs,    2026-07-13, 4d
    User acceptance testing         :         p2_uat,    2026-07-20, 5d
    MVP release                     :milestone, p2_done, 2026-07-24, 0d

    section Phase 3 – Expansion
    Multi-source ingestion          :         p3_src,    2026-07-27, 8d
    Hybrid search (BM25 + dense)    :         p3_hyb,    2026-08-03, 7d
    Feedback loop                   :         p3_fb,     2026-08-10, 5d
    CI/CD pipeline                  :         p3_cicd,   2026-08-10, 6d
    Advanced chunking               :         p3_chunk,  2026-08-17, 5d
    React/Next.js UI                :         p3_ui,     2026-08-17, 10d
    Runbook & on-call docs          :         p3_docs,   2026-08-31, 5d
    Post-launch review              :milestone, p3_done, 2026-09-18, 0d
```

## Timeline Summary

| Phase | Start | End | Duration |
|---|---|---|---|
| 0 – Environment Setup | Week 1 (2026-05-04) | Week 2 (2026-05-15) | 2 weeks |
| 1 – POC | Week 3 (2026-05-18) | Week 6 (2026-06-12) | 4 weeks |
| 2 – MVP | Week 7 (2026-06-15) | Week 12 (2026-07-24) | 6 weeks |
| 3 – Expansion | Week 13 (2026-07-27) | Week 20 (2026-09-18) | 8 weeks |
| **Total** | | | **20 weeks** |

## Key Milestones

| Milestone | Date |
|---|---|
| Phase 0 complete – platform ready | 2026-05-15 |
| Phase 1 complete – POC validated | 2026-06-12 |
| Phase 2 complete – MVP live | 2026-07-24 |
| Phase 3 complete – full system | 2026-09-18 |
