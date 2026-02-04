# Workaround for texlive 但是为啥 python3-wordcloud 需要texlive-base？
FROM texlive/texlive:latest-basic

ENV LANG en_US.UTF-8

ENV PANDOC_VERSION=3.8.3
ENV PANDOC_CROSSREF_VERSION=0.3.23

WORKDIR /opt/dependencies

RUN apt-get update && apt-get upgrade -y

# Util tools
RUN apt-get install -y --no-install-recommends apt-utils
RUN apt-get install -y --no-install-recommends sudo ca-certificates pkg-config curl wget bzip2 make gnupg
RUN apt-get install -y --no-install-recommends git git-restore-mtime
RUN apt-get install -y --no-install-recommends zip unzip xz-utils
RUN apt-get install -y --no-install-recommends locales
RUN apt-get install -y --no-install-recommends ssh patch
RUN apt-get install -y --no-install-recommends python3 python3-pip
RUN apt-get install -y --no-install-recommends gcc g++ make imagemagick

# python
RUN apt-get install -y --no-install-recommends python3-wordcloud
# debian 没有 libjpeg8-dev
RUN apt-get install -y --no-install-recommends libpixman-1-dev libcairo2-dev libpango1.0-dev libgif-dev

RUN apt-get clean

# pandoc
RUN \
    wget -q https://github.com/jgm/pandoc/releases/download/${PANDOC_VERSION}/pandoc-${PANDOC_VERSION}-linux-amd64.tar.gz; \
    tar zxf pandoc-${PANDOC_VERSION}-linux-amd64.tar.gz; \
    mv pandoc-${PANDOC_VERSION}/bin/pandoc pandoc; \
    wget -q https://github.com/lierdakil/pandoc-crossref/releases/download/v${PANDOC_CROSSREF_VERSION}/pandoc-crossref-Linux-X64.tar.xz; \
    tar xf pandoc-crossref-Linux-X64.tar.xz; \
    rm -rf ./*tar* pandoc-${PANDOC_VERSION}

# 创建 dev 用户（供本地 devcontainer 使用；CI 默认用 root）
ARG USERNAME=texlive
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME 2>/dev/null || true && \
    useradd --uid $USER_UID --gid $USER_GID -m -s /bin/bash $USERNAME 2>/dev/null || true && \
    echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

USER $USERNAME

RUN echo "export PATH=\$PATH:/opt/dependencies" >>$HOME/.bashrc

# nvm
ARG NODE_VER=24
SHELL ["/bin/bash", "--login", "-i", "-c"]
RUN wget -qO- https://raw.githubusercontent.com/nvm-sh/nvm/master/install.sh | bash
RUN source $HOME/.bashrc && nvm install $NODE_VER && nvm alias default $NODE_VER
SHELL ["/bin/bash", "--login", "-c"]

# 镜像默认以 root 运行，供 GitHub Action deploy 使用；本地 devcontainer 通过 remoteUser 使用 texlive
USER root
