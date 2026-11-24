# Environnement commun : compilation C, tests, valgrind, scripts Python (OSM, figures).
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      build-essential gdb valgrind \
      python3 python3-venv \
      ca-certificates git \
 && rm -rf /var/lib/apt/lists/*

# Certificat racine supplémentaire, facultatif (réseau d'entreprise qui inspecte le TLS).
# Usage : docker build --secret id=extra_ca,src=mon_ca.pem -t lro-dev:1 .
RUN --mount=type=secret,id=extra_ca,required=false \
    if [ -s /run/secrets/extra_ca ]; then \
      cp /run/secrets/extra_ca /usr/local/share/ca-certificates/extra.crt && update-ca-certificates; \
    fi

# pip et requests (OSMnx) utilisent le magasin de certificats du système
ENV PIP_CERT=/etc/ssl/certs/ca-certificates.crt \
    REQUESTS_CA_BUNDLE=/etc/ssl/certs/ca-certificates.crt

# Dépendances Python dans un venv, versions figées dans scripts/requirements.txt
COPY scripts/requirements.txt /tmp/requirements.txt
RUN python3 -m venv /opt/venv \
 && /opt/venv/bin/pip install --no-cache-dir -r /tmp/requirements.txt

ENV PATH="/opt/venv/bin:$PATH" \
    HOME=/tmp \
    MPLCONFIGDIR=/tmp/matplotlib

WORKDIR /work
CMD ["bash"]
