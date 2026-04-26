# Phase 1 – Proof of Concept (POC)

**Duration:** 4 weeks (Weeks 3–6)  
**Owner:** Developer  
**Prerequisite:** Phase 0 acceptance criteria met  
**Goal:** Validate the RAG approach on a real (but limited) document corpus. Prove quality before investing in infrastructure.

---

## Scope

- **Corpus:** One team's documentation (e.g., internal wiki exports, Markdown files, PDFs). Target: 50–200 documents.
- **Interface:** Command-line only — no UI required at this stage.
- **Infrastructure:** Runs locally on the bare-metal machine (no Kubernetes yet).
- **Evaluation:** Automated quality metrics (RAGAS) on a hand-crafted 20-question gold-set.

---

## Architecture

```
┌──────────────────────────────────────────────────┐
│                   POC Pipeline                   │
│                                                  │
│  Raw Docs  →  Chunker  →  Embedder  →  Qdrant    │
│                                    (vector store) │
│                                          │        │
│  User Query  →  Embedder  →  Retriever──┘        │
│                                   │              │
│                               Top-k chunks       │
│                                   │              │
│                             LLM (Ollama)          │
│                                   │              │
│                          Answer + Citations       │
└──────────────────────────────────────────────────┘
```

---

## Task List

### 1.1 — Start Qdrant Locally

```bash
docker run -d --name qdrant \
  -p 6333:6333 -p 6334:6334 \
  -v $(pwd)/qdrant_storage:/qdrant/storage \
  qdrant/qdrant
```

### 1.2 — Document Ingestion Pipeline

Create `ingest.py`:

```python
from langchain_community.document_loaders import DirectoryLoader
from langchain.text_splitter import RecursiveCharacterTextSplitter
from langchain_community.embeddings import OllamaEmbeddings
from langchain_community.vectorstores import Qdrant

DOCS_DIR = "./docs-raw"
QDRANT_URL = "http://localhost:6333"
COLLECTION = "rag-poc"

loader = DirectoryLoader(DOCS_DIR, glob="**/*.{md,pdf,txt}", show_progress=True)
docs = loader.load()

splitter = RecursiveCharacterTextSplitter(chunk_size=512, chunk_overlap=64)
chunks = splitter.split_documents(docs)

embeddings = OllamaEmbeddings(model="nomic-embed-text", base_url="http://localhost:11434")

Qdrant.from_documents(
    chunks,
    embeddings,
    url=QDRANT_URL,
    collection_name=COLLECTION,
    force_recreate=True,
)
print(f"Ingested {len(chunks)} chunks from {len(docs)} documents.")
```

Run:

```bash
source ~/.venv/rag/bin/activate
python ingest.py
```

### 1.3 — Retrieval + Generation Chain

Create `query.py`:

```python
import sys
from langchain_community.embeddings import OllamaEmbeddings
from langchain_community.vectorstores import Qdrant
from langchain_community.llms import Ollama
from langchain.chains import RetrievalQAWithSourcesChain
from qdrant_client import QdrantClient

QDRANT_URL = "http://localhost:6333"
COLLECTION = "rag-poc"
LLM_MODEL  = "mistral:7b-instruct-q4_K_M"

client = QdrantClient(url=QDRANT_URL)
embeddings = OllamaEmbeddings(model="nomic-embed-text")
vectorstore = Qdrant(client=client, collection_name=COLLECTION, embeddings=embeddings)
retriever = vectorstore.as_retriever(search_kwargs={"k": 5})

llm = Ollama(model=LLM_MODEL, base_url="http://localhost:11434")
chain = RetrievalQAWithSourcesChain.from_chain_type(llm=llm, retriever=retriever)

query = " ".join(sys.argv[1:]) or "What is the deployment process?"
result = chain({"question": query})
print("\nAnswer:", result["answer"])
print("\nSources:", result["sources"])
```

Run:

```bash
python query.py "How do I reset a user password?"
```

### 1.4 — Build the Gold-Set

Create `gold_set.json` — a JSON array of 20 question/answer pairs derived from the corpus. Example:

```json
[
  {
    "question": "What is the on-boarding procedure for new engineers?",
    "ground_truth": "New engineers must complete ...",
    "source": "docs-raw/onboarding.md"
  }
]
```

### 1.5 — RAGAS Evaluation

```python
# evaluate.py
from datasets import Dataset
from ragas import evaluate
from ragas.metrics import faithfulness, answer_relevancy, context_recall
import json, sys

# Load gold set
with open("gold_set.json") as f:
    gold = json.load(f)

# Run each question through the RAG chain (re-use query.py logic)
# ... collect answers, contexts into lists ...

dataset = Dataset.from_dict({
    "question":  [g["question"]  for g in results],
    "answer":    [g["answer"]    for g in results],
    "contexts":  [g["contexts"]  for g in results],
    "ground_truth": [g["ground_truth"] for g in results],
})

scores = evaluate(dataset, metrics=[faithfulness, answer_relevancy, context_recall])
print(scores)
```

Record the scores in `docs/rag-agent/evaluation-poc.md`.

### 1.6 — POC Retrospective

Hold a (solo) retrospective and document:

1. Quality scores vs. targets
2. Main failure modes (hallucinations, missing context, wrong chunk size)
3. Proposed improvements for MVP
4. Go / No-go decision

Template: `docs/rag-agent/evaluation-poc.md`

---

## Acceptance Criteria

| Criterion | Target |
|---|---|
| Ingestion pipeline processes all pilot docs without errors | 100 % |
| CLI returns an answer with at least one cited source for each gold-set question | 100 % |
| RAGAS answer relevancy | ≥ 0.70 |
| RAGAS faithfulness | ≥ 0.75 |
| POC retrospective document written | ✅ |

---

## Tips & Pitfalls

- **Chunk size matters:** Start with 512 tokens, overlap 64. If retrieval misses context that spans two paragraphs, increase to 1024.
- **Embedding model consistency:** The same model must be used at ingest time and at query time.
- **Cold GPU start:** First Ollama call after boot may take 10–20 s while the model loads into VRAM. Subsequent calls are fast (~1–3 s for generation at 7B).
- **PDF extraction quality:** Scanned PDFs require OCR. Use `unstructured[paddleocr]` or `docling` for better fidelity.
- **Context window:** Mistral 7B context is 32 k tokens. With top-5 chunks of 512 tokens each (≈2.5 k tokens total), you have ample room.
