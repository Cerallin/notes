---
disableNunjucks: true
mathjax: true
title: 一种利用GitHub Actions自动部署博客的方案
tags:
  - GitHub
  - hexo
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了作者通过GitHub
  Action实现Hexo静态博客的自动部署，包括环境配置、自动同步改进和RSA密钥认证的步骤。文中提到具体的操作流程，如使用rsync优化文件传输速度，并将本地Hexo项目与远程仓库进行管理与部署。作者还分享了如何通过脚本处理小说网站文本格式以适配Markdown语法。
date: 2021-12-13 18:16:00
---

>本博客暂时没有公开git仓库的打算，但可以展示一下我的GitHub Action配置。

<!-- more -->

GitHub action的准备阶段如下：指定push到master分支时触发，指定环境为ubuntu-latest。

```yml
name: Deploy Hexo

on: [push]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - name: Checkout
      uses: actions/checkout@v2
      with:
        ref: master
```

安装hexo主题：`hexo-theme-yuzu`。

```yml
    - name: Update Submodule
      run: |
        git submodule init
        git submodule update --remote
```

安装pandoc和pandoc-crossref，合并所有bib文件。

```yml
    - name: Install pandoc with crossref
      run: |
        wget -q https://github.com/jgm/pandoc/releases/download/2.16.2/pandoc-2.16.2-linux-amd64.tar.gz
        tar zxf pandoc-2.16.2-linux-amd64.tar.gz
        mv pandoc-2.16.2/bin/pandoc pandoc
        wget -q https://github.com/lierdakil/pandoc-crossref/releases/download/v0.3.12.1a/pandoc-crossref-Linux.tar.xz
        tar xf pandoc-crossref-Linux.tar.xz
        echo "$PWD" >> $GITHUB_PATH

    - name: Generate bib file
      run: cat ./source/_bibs/*.bib >./bibfile.bib
```

安装nodejs环境。

```yml
    - name: Setup Node
      uses: actions/setup-node@v1
      with:
        node-version: "12"
```

生成静态文件并部署。

```yml
    - name: Hexo Generate
      run: |
        rm -rf ./node_modules
        npm install
        npm run clean
        npm run build

    # 已弃用，见下文
    - name: SFTP Deploy
      env:
        SSH_PRIVATE: ${{ secrets.SSH_PRIVATE }}
      uses: wlixcc/SFTP-Deploy-Action@v1.2.1
      with:
        username: ubuntu
        port: 21212
        server: www.cerallin.top
        ssh_private_key: ${{ secrets.SSH_PRIVATE }}
        local_path: ./public/*
        remote_path: /var/www/notes.cerallin.top
        args: '-o ConnectTimeout=5'
```

**更新**

后来我发现sftp太慢了，因为美国访问我的腾讯云服务器本来ttl就长，再加上所有文件都要传输一遍。但是众所周知，大多数文件并不会在每次渲染时都更新，因此我把最后一个step改成了基于rsync的同步。

```yml
    - name: Hexo Deploy
      id: deploy
      uses: Pendect/action-rsyncer@v1.1.0
      env:
        DEPLOY_KEY: ${{ secrets.SSH_PRIVATE }}
      with:
        flags: '-avzrc --delete'
        options: ''
        src: 'public/'
        dest: 'cerallin@host:/path/to/notes'

    - name: Display deploy status
      run: echo "${{ steps.deploy.outputs.status }}"
```

**更新**

再后来我把我的小说站和主题展示网站的部署全部自动化了。

我的[cerallin.github.io](https://cerallin.github.io)是hexo生成后的静态网址。
虽然可以在`xxx.github.io` repo里直接上传hexo的整个配置，但我并不想把我的`_config.yml`暴露给别人。
好在hexo早就提供了基于git的部署方案：
```yml
# Deployment
## Docs: https://hexo.io/docs/one-command-deployment
deploy:
  - type: 'git'
    repo: git@github.com:cerallin/cerallin.github.io
    branch: master
```

这样配置的好处是，我的hexo在本地修改，然后运行`hexo deploy`即可上传到`cerallin.github.io`仓库。
在此基础上，我又把本地hexo用git管理起来，并使用GitHub Action自动部署。

为此，首先需要生成一对RSA密钥，以实现自动化部署时的身份认证。

首先设置`cerallin.github.io`仓库的`Deploy keys`为公钥；
然后设置私有仓库的`secrets.SSH_KEY`为私钥。
最后配置私有仓库的GitHub Action，其（局部）配置如下：

```yml
- steps:
  # ... 前置任务
  - name: Setup Deploy Key
    env:
      HEXO_DEPLOY_KEY: ${{ secrets.SSH_KEY }}
    run: |
      mkdir -p ~/.ssh/
      echo "$HEXO_DEPLOY_KEY" > ~/.ssh/id_rsa
      chmod 700 ~/.ssh
      chmod 600 ~/.ssh/id_rsa
      ssh-keyscan github.com >> ~/.ssh/known_hosts
      npm install hexo-cli -g
      npm install

  - name: Deploy
    run: |
      npm run clean
      npm run deploy

```

**P.S.** 配套的本地提交git的shell：

```sh
blog-update() {
	# Remove text indents
	sed -i "s_^　　_\n_" ./source/_posts/*.md
	# Commit all
	git add -A && git commit -m "Updated at $(TZ=Asia/Shanghai date +'%F %H:%M:%S')"
	git push
}
```

其中，一开始的sed指令是为了适配我的小说网站，从Pure Writer复制粘贴来的文本每段开头自带两个全角空格，使用sed指令将它们全部转换为换行，这样在Markdown语法中自然形成分段。

[Fin.]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
