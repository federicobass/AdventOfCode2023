# Phase 0 – Environment Setup

**Duration:** 2 weeks (Weeks 1–2)  
**Owner:** Developer  
**Goal:** A fully functional local platform — GPU-enabled LLM serving, Kubernetes running, Cline integrated.

---

## Prerequisites

- Physical access to the Ubuntu machine
- Internet connectivity for initial package downloads (subsequent phases are fully offline)
- Ubuntu 22.04 LTS ISO (or existing installation)

---

## Task List

### 0.1 — OS & System Configuration

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install build essentials
sudo apt install -y build-essential curl wget git vim htop nvtop

# Set hostname (optional)
sudo hostnamectl set-hostname rag-server
```

### 0.2 — Nvidia Driver & CUDA

```bash
# Add graphics driver PPA
sudo add-apt-repository ppa:graphics-drivers/ppa -y
sudo apt update

# Install recommended driver (check with: ubuntu-drivers devices)
sudo apt install -y nvidia-driver-535

# Reboot and verify
sudo reboot
nvidia-smi
```

Expected output: RTX A2000 listed, driver version ≥ 525, CUDA Version ≥ 12.0.

Install CUDA Toolkit (for potential fine-tuning or custom kernels):

```bash
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.1-1_all.deb
sudo dpkg -i cuda-keyring_1.1-1_all.deb
sudo apt update
sudo apt install -y cuda-toolkit-12-3
echo 'export PATH=/usr/local/cuda/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

### 0.3 — Docker Engine

```bash
# Install Docker
curl -fsSL https://get.docker.com | sudo sh
sudo usermod -aG docker $USER
newgrp docker

# Install Nvidia Container Toolkit
curl -fsSL https://nvidia.github.io/libnvidia-container/gpgkey | sudo gpg --dearmor -o /usr/share/keyrings/nvidia-container-toolkit-keyring.gpg
curl -s -L https://nvidia.github.io/libnvidia-container/stable/deb/nvidia-container-toolkit.list | \
  sed 's#deb https://#deb [signed-by=/usr/share/keyrings/nvidia-container-toolkit-keyring.gpg] https://#g' | \
  sudo tee /etc/apt/sources.list.d/nvidia-container-toolkit.list
sudo apt update && sudo apt install -y nvidia-container-toolkit
sudo nvidia-ctk runtime configure --runtime=docker
sudo systemctl restart docker
```

### 0.4 — k3s (Lightweight Kubernetes)

```bash
# Install k3s (single-node, with Nvidia device plugin support)
curl -sfL https://get.k3s.io | INSTALL_K3S_EXEC="--disable traefik" sh -

# Configure kubectl for current user
mkdir -p ~/.kube
sudo cp /etc/rancher/k3s/k3s.yaml ~/.kube/config
sudo chown $USER:$USER ~/.kube/config
export KUBECONFIG=~/.kube/config
echo 'export KUBECONFIG=~/.kube/config' >> ~/.bashrc

# Verify
kubectl get nodes
```

Install Nvidia GPU Operator so pods can request GPU resources:

```bash
helm repo add nvidia https://helm.ngc.nvidia.com/nvidia
helm repo update
helm install gpu-operator nvidia/gpu-operator \
  --namespace gpu-operator \
  --create-namespace \
  --set driver.enabled=false   # driver already installed on host
```

### 0.5 — Helm

```bash
curl https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3 | bash
helm version
```

### 0.6 — Ollama

```bash
# Install Ollama on the host (used during POC; later containerised in MVP)
curl -fsSL https://ollama.com/install.sh | sh

# Pull the recommended model
ollama pull mistral:7b-instruct-q4_K_M

# Pull the embedding model
ollama pull nomic-embed-text

# Smoke test
ollama run mistral:7b-instruct-q4_K_M "Hello, confirm you are running."
```

### 0.7 — Python Environment

```bash
sudo apt install -y python3.11 python3.11-venv python3-pip
python3.11 -m venv ~/.venv/rag
source ~/.venv/rag/bin/activate
pip install --upgrade pip

# Core RAG dependencies (used from Phase 1 onwards)
pip install langchain langchain-community langchain-ollama \
            qdrant-client sentence-transformers \
            ragas datasets unstructured[pdf] docling \
            fastapi uvicorn streamlit
```

### 0.8 — Cline Integration

1. Install VS Code on the developer workstation (may be the same machine or a laptop).
2. Install the **Cline** extension from the VS Code marketplace.
3. In VS Code `settings.json`, point Cline at the local Ollama server:

```json
{
  "cline.apiProvider": "ollama",
  "cline.ollamaBaseUrl": "http://<machine-ip>:11434",
  "cline.ollamaModelId": "mistral:7b-instruct-q4_K_M"
}
```

4. Open a new Cline chat and confirm a response is received.

---

## Acceptance Criteria

| Check | Command | Expected result |
|---|---|---|
| GPU visible | `nvidia-smi` | RTX A2000, no errors |
| CUDA available | `nvcc --version` | CUDA 12.x |
| Docker GPU access | `docker run --gpus all nvidia/cuda:12.3.0-base-ubuntu22.04 nvidia-smi` | GPU listed |
| Kubernetes running | `kubectl get nodes` | Node `Ready` |
| Ollama serving | `curl http://localhost:11434/api/tags` | JSON with model list |
| Cline connected | VS Code → Cline chat | Response from local LLM |

---

## Notes

- If the machine is headless, manage it via SSH. Use `tmux` for persistent sessions.
- Keep a snapshot of the disk (or document exact commands used) to allow full reproducibility.
- k3s stores cluster data under `/var/lib/rancher/k3s`; include this in any backup strategy.
