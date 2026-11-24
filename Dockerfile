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
# Fourni par compose.yaml via la variable EXTRA_CA_FILE (fichier vide par défaut)
RUN --mount=type=secret,id=extra_ca,required=false \
    if [ -s /run/secrets/extra_ca ]; then \
      cp /run/secrets/extra_ca /usr/local/share/ca-certificates/extra.crt && update-ca-certificates; \
    fi

# pip et requests (OSMnx) utilisent le magasin de certificats du système
ENV PIP_CERT=/etc/ssl/certs/ca-certificates.crt \
    REQUESTS_CA_BUNDLE=/etc/ssl/certs/ca-certificates.crt

# Dépendances Python dans un venv : requirements.txt liste ce qu'on utilise,
# requirements.lock fige toutes les versions (dépendances indirectes comprises)
COPY scripts/requirements.txt scripts/requirements.lock /tmp/
RUN python3 -m venv /opt/venv \
 && /opt/venv/bin/pip install --no-cache-dir -r /tmp/requirements.txt -c /tmp/requirements.lock

ENV PATH="/opt/venv/bin:$PATH" \
    HOME=/tmp \
    MPLCONFIGDIR=/tmp/matplotlib

WORKDIR /work
CMD ["bash"]
