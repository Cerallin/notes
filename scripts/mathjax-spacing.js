const hanzi = '[\u2E80-\u2FFF\u31C0-\u31EF\u3300-\u4DBF\u4E00-\u9FFF\uF900-\uFAFF\uFE30-\uFE4F]';
const mathjax = '<span class="math inline">.*?</span>';

function matchReplace(str) {
  return str
    .replace(RegExp(`(${hanzi})(${mathjax})`, 'g'), '$1<hl></hl>$2')
    .replace(RegExp(`(${mathjax})(${hanzi})`, 'g'), '$1<hl></hl>$2');
}

hexo.extend.filter.register('after_render:html', matchReplace);
