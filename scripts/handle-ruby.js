/**
 * 将「注音链接」转为 HTML <ruby> 标签。
 * 约定：Markdown 中 [显示文字](-注音) 在渲染前被替换为 <ruby>…</ruby>，Pandoc 会按原生 HTML 输出。
 */

function decodeHref(value) {
  try {
    return decodeURIComponent(value);
  } catch {
    return value;
  }
}

function handleRubyInMarkdown(content) {
  return content.replace(
    /\[([^\]]+)\]\(-([^)]+)\)/g,
    (_, text, rubyEncoded) => {
      const ruby = decodeHref(rubyEncoded);
      return `<ruby>${text}<rp>(</rp><rt>${ruby}</rt><rp>)</rp></ruby>`;
    }
  );
}

hexo.extend.filter.register('before_post_render', (data) => {
  data.content = handleRubyInMarkdown(data.content);
  return data;
});
