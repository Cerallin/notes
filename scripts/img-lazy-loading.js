/**
 * 为图片添加 loading="lazy" 属性。
 */

function addLazyLoading(html) {
  return html.replace(
    /<img(\s*)(?![^>]*\bloading\s*=)/gi,
    '<img loading="lazy"$1'
  );
}

hexo.extend.filter.register('after_render:html', addLazyLoading);
