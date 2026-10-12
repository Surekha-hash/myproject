
#!/usr/bin/env bash
set -Eeuo pipefail

# DevOps Lab Setup
# Target: Ubuntu Server 24.04 LTS on EC2
# Installs: Git, Docker, Compose, Java 21, Jenkins LTS, Trivy, Python 3
# Kubernetes is intentionally excluded.

if [[ "${EUID}" -eq 0 ]]; then
    echo "Please run this script as your normal Ubuntu user, not as root."
    echo "Example: ./setup-devops.sh"
    exit 1
fi

echo "======================================"
echo " Starting DevOps Lab Installation"
echo "======================================"

echo "=== Checking Ubuntu version ==="
if ! grep -q 'VERSION_ID="24.04"' /etc/os-release; then
    echo "Warning: This script is intended for Ubuntu 24.04 LTS."
fi

echo "=== Installing prerequisites and Git ==="
sudo apt-get update
sudo apt-get install -y \
    ca-certificates \
    curl \
    gnupg \
    wget \
    fontconfig \
    openjdk-21-jre \
    git \
    python3

echo "=== Installing Docker Engine and Compose ==="
sudo install -m 0755 -d /etc/apt/keyrings

sudo curl -fsSL \
    https://download.docker.com/linux/ubuntu/gpg \
    -o /etc/apt/keyrings/docker.asc

sudo chmod a+r /etc/apt/keyrings/docker.asc

. /etc/os-release

echo "Types: deb
URIs: https://download.docker.com/linux/ubuntu
Suites: ${UBUNTU_CODENAME:-$VERSION_CODENAME}
Components: stable
Architectures: $(dpkg --print-architecture)
Signed-By: /etc/apt/keyrings/docker.asc" \
    | sudo tee /etc/apt/sources.list.d/docker.sources >/dev/null

sudo apt-get update

sudo apt-get install -y \
    docker-ce \
    docker-ce-cli \
    containerd.io \
    docker-buildx-plugin \
    docker-compose-plugin

sudo systemctl enable --now docker

echo "=== Installing Jenkins LTS ==="
sudo install -m 0755 -d /usr/share/keyrings

sudo curl -fsSL \
    https://pkg.jenkins.io/debian-stable/jenkins.io-2026.key \
    -o /usr/share/keyrings/jenkins-keyring.asc

echo "deb [signed-by=/usr/share/keyrings/jenkins-keyring.asc] https://pkg.jenkins.io/debian-stable binary/" \
    | sudo tee /etc/apt/sources.list.d/jenkins.list >/dev/null

sudo apt-get update
sudo apt-get install -y jenkins
sudo systemctl enable --now jenkins

echo "=== Installing Trivy ==="
sudo curl -fsSL \
    https://get.trivy.dev/deb/public.key \
    | sudo gpg --dearmor --yes \
        -o /usr/share/keyrings/trivy.gpg

echo "deb [signed-by=/usr/share/keyrings/trivy.gpg] https://get.trivy.dev/deb generic main" \
    | sudo tee /etc/apt/sources.list.d/trivy.list >/dev/null

sudo apt-get update
sudo apt-get install -y trivy

echo "=== Configuring Docker access ==="
# This allows the Ubuntu user to run Docker without sudo.
sudo usermod -aG docker "$USER"

# Jenkins needs Docker access to build images in the pipeline.
# WARNING: membership in the docker group grants root-equivalent privileges.
sudo usermod -aG docker jenkins

sudo systemctl restart docker
sudo systemctl restart jenkins

echo
echo "======================================"
echo " Installation and verification"
echo "======================================"

git --version
docker --version
docker compose version
java -version
python3 --version
trivy --version

echo
echo "=== Docker service ==="
sudo systemctl is-active docker

echo
echo "=== Jenkins service ==="
sudo systemctl is-active jenkins

echo
echo "=== Disk space ==="
df -h /

echo
echo "=== Jenkins initial administrator password ==="
if sudo test -f /var/lib/jenkins/secrets/initialAdminPassword; then
    sudo cat /var/lib/jenkins/secrets/initialAdminPassword
else
    echo "Password file not available yet. Check Jenkins service status."
fi

echo
echo "======================================"
echo " Installation completed!"
echo "======================================"
echo "Next steps:"
echo "1. Exit SSH and reconnect."
echo "2. Verify Docker using: docker run hello-world"
echo "3. Open Jenkins at http://YOUR_EC2_PUBLIC_IP:8080"
echo "4. Complete the Jenkins setup wizard."
echo "5. Clone your project and run your pipeline."
echo
echo "Remember to allow TCP port 8080 from your own IP"
echo "in the EC2 security group. Do not expose it to everyone."
