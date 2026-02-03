hexo.extend.filter.register('after_render:html',
  str => str.replace(/<figure class="highlight (c|cpp)">(.*?)<\/figure>/g,
    fancyCodeBlock => fancyCodeBlock
      .replace(/<span class="string">"(.*?)"<\/span>/g,
        stringSnippet => stringSnippet.replace(
          /(\\n|%[0-9]*?\.*?[0-9]*?l?[dfuocsx])/g, '<span class="variable">$1</span>'))
      .replace(
        /#<span class="keyword">(include|define|if|endif)<\/span>/g,
        '<span class="keyword">#$1</span>')))
