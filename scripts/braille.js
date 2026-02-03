// Impact by https://github.com/Zwettekop/BrailleToSVG/blob/main/script.js

const scale = 4;
const dotSpacing = 2.5 * scale;
const letterSpacing = 7 * scale;
const dotSize = 0.8 * scale;
const lineSpacing = 11 * scale;

// braille(123, 45)
const braillePattern = /braille\((\d+(?:,\s*\d+)*)\)/g;

function renderSingleCharacter(pattern = '123', x, y) {
  const mapping = [..."123456"].map(n => ([...pattern].indexOf(n) > -1) ? true : false);
  return mapping.map((fill, index) => {
    const cx = (index >= 3) ? dotSpacing : 0;
    const cy = (index % 3) * dotSpacing;
    return `<circle fill="${fill ? '#000' : 'none'}" stroke="#000" cx="${x + cx}" cy="${y + cy}" r="${dotSize}"></circle>`;
  });
}

function braille(args) {
  const figCaption = args.shift();
  const arr = args.filter(s => s !== '').map(s => s.trim());
  const count = arr.length;

  let circles = [];
  let x = 25;
  let y = 20;
  for (const pattern of arr) {
    if (pattern === "0") {
      x += letterSpacing / 2;
    } else {
      circles = circles.concat(renderSingleCharacter(pattern, x, y));
      x += letterSpacing;
    }
  }
  const svgString = [
    [
      '<svg id="svgBraille"',
      'xmlns="http://www.w3.org/2000/svg"',
      'xmlns:xlink="http://www.w3.org/1999/xlink"',
      `width="${ x - letterSpacing + dotSpacing + 25 }px" height="${ 40 + 2 * dotSpacing }px">`,
    ].join(' '),

    circles.join(''),

    '</svg>',
  ].join('');

  const svgBase64 = Buffer.from(svgString).toString('base64')

  return [
    '<figure class="braille-series">',
    `<img src="data:image/svg+xml;base64,${svgBase64}" alt="braille(${args.join(', ')})">`,
    ((figCaption !== '-') ? `<figcaption aria-hidden="true">${figCaption}</figcaption>` : ''),
    '</figure>',
  ].join('')
}

hexo.extend.tag.register('braille', braille);
