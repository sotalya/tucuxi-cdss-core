// SPDX-License-Identifier: AGPL-3.0-or-later
function CgDrawAxis(cdata, ctx) {
  var arrowSize = 5 * cdata.scale;
  var overrun = 10 * cdata.scale;
  ctx.beginPath();
  ctx.moveTo(cdata.bottomLeftX, cdata.bottomLeftY);
  ctx.lineTo(cdata.bottomRightX + overrun, cdata.bottomRightY);
  ctx.stroke();
  ctx.beginPath();
  ctx.moveTo(cdata.bottomLeftX, cdata.bottomLeftY);
  ctx.lineTo(cdata.topLeftX, cdata.topLeftY - overrun);
  ctx.stroke();
  ctx.beginPath();
  ctx.moveTo(cdata.bottomRightX + overrun, cdata.bottomRightY);
  ctx.lineTo(
    cdata.bottomRightX + overrun - arrowSize,
    cdata.bottomRightY - arrowSize,
  );
  ctx.lineTo(
    cdata.bottomRightX + overrun - arrowSize,
    cdata.bottomRightY + arrowSize,
  );
  ctx.closePath();
  ctx.stroke();
  ctx.fill();
  ctx.beginPath();
  ctx.moveTo(cdata.topLeftX, cdata.topLeftY - overrun);
  ctx.lineTo(cdata.topLeftX - arrowSize, cdata.topLeftY - overrun + arrowSize);
  ctx.lineTo(cdata.topLeftX + arrowSize, cdata.topLeftY - overrun + arrowSize);
  ctx.closePath();
  ctx.stroke();
  ctx.fill();
}
function CgAcxn2screen(cdata, c) {
  var result = -(c - cdata.minY) * cdata.yRatio + cdata.bottomLeftY;
  return result;
}
function CgDrawAxisTicks(cdata, ctx) {
  ctx.textAlign = "left";
  ctx.textBaseline = "middle";
  ctx.font = cdata.axisTicksFontSize + " " + cdata.police;
  var tickSize = 10 * cdata.scale;
  var dateTickSize = 30 * cdata.scale;
  var tickSpacingy = 0.2 * cdata.scale;
  var down = -1.0;
  var up = 1.0;
  var down_i = Math.ceil(down / tickSpacingy);
  var up_i = Math.floor(up / tickSpacingy);
  var remy = down_i * tickSpacingy - down;
  var firstTickY = -1.0 + remy;
  var tickOffsetY = cdata.bottomLeftY - 1.0 - firstTickY;
  var tickIncrementY = (tickSpacingy * cdata.plotHeight) / 2;
  var nbTicksY = up_i - down_i + 1;
  if (nbTicksY > 21) nbTicksY = 21;
  var order = 2 - Math.log(cdata.maxY - cdata.minY) / Math.LN10;
  if (order >= 1 && order < 4) {
    cdata.unitefforder = 1e3;
    cdata.unit = "ng/l";
  }
  if (order >= 4 && order < 7) {
    cdata.unitefforder = 1e6;
    cdata.unit = "pg/l";
  }
  if (order > -5 && order < -2) {
    cdata.unitefforder = 1e-3;
    cdata.unit = "mg/l";
  }
  if (order > -8 && order < -5) {
    cdata.unitefforder = 1e-6;
    cdata.unit = "g/l";
  }
  if (order < 1 && order >= -2) {
    cdata.unitefforder = 1;
    cdata.unit = "ug/l";
  }
  for (var i = 0; i < nbTicksY; i++) {
    var y = tickOffsetY - i * tickIncrementY;
    var tickScaleY = (i + down_i) % 10 ? 0.5 : 1;
    ctx.beginPath();
    ctx.moveTo(cdata.bottomLeftX, y);
    ctx.lineTo(cdata.bottomLeftX - tickSize * tickScaleY, y);
    ctx.stroke();
    ctx.textAlign = "right";
    ctx.textBaseline = "middle";
    var val = cdata.minY + 0.5 * i * tickSpacingy * (cdata.maxY - cdata.minY);
    ctx.fillText(
      Math.max(0, Math.round(cdata.unitefforder * val)),
      cdata.bottomLeftX - tickSize * 1.5,
      y,
    );
  }
  var x1 = cdata.bottomLeftX + (cdata.bottomRightX - cdata.bottomLeftX) * 0.25;
  var x2 = cdata.bottomLeftX + (cdata.bottomRightX - cdata.bottomLeftX) * 0.75;
  ctx.beginPath();
  ctx.moveTo(x1, cdata.bottomLeftY);
  ctx.lineTo(x1, cdata.bottomLeftY + tickSize);
  ctx.stroke();
  ctx.beginPath();
  ctx.moveTo(x2, cdata.bottomLeftY);
  ctx.lineTo(x2, cdata.bottomLeftY + tickSize);
  ctx.stroke();
  var xmiddle =
    cdata.bottomLeftX + (cdata.bottomRightX - cdata.bottomLeftX) * 0.5;
  ctx.save();
  ctx.strokeStyle = "red";
  ctx.lineWidth = 4.0 * cdata.scale;
  var space = 3;
  ctx.setLineDash([3 * space, space, 3 * space, space]);
  ctx.beginPath();
  ctx.moveTo(xmiddle, cdata.bottomLeftY);
  ctx.lineTo(xmiddle, cdata.topLeftY);
  ctx.stroke();
  ctx.restore();
  ctx.save();
  ctx.font = "bold " + cdata.fontSize + " " + cdata.police;
  ctx.textAlign = "center";
  ctx.textBaseline = "middle";
  ctx.fillText(
    cdata.data.textRecommendation,
    xmiddle,
    cdata.topLeftY - 15 * cdata.scale,
  );
  ctx.textAlign = "center";
  ctx.textBaseline = "middle";
  ctx.fillText(cdata.data.textBefore, x1, cdata.topLeftY - 15 * cdata.scale);
  ctx.textAlign = "center";
  ctx.textBaseline = "middle";
  ctx.fillText(cdata.data.textAfter, x2, cdata.topLeftY - 15 * cdata.scale);
  ctx.restore();
}
function CgDrawAxisLabels(cdata, ctx) {
  ctx.save();
  ctx.font = "bold " + cdata.fontSize + " " + cdata.police;
  ctx.textAlign = "left";
  ctx.textBaseline = "middle";
  ctx.fillText(
    cdata.data.textTime,
    cdata.leftMargin + cdata.plotWidth + 15 * cdata.scale,
    cdata.topMargin + cdata.plotHeight,
  );
  ctx.textAlign = "center";
  ctx.textBaseline = "bottom";
  ctx.fillText(
    cdata.unit,
    cdata.leftMargin,
    cdata.topMargin - 15 * cdata.scale,
  );
  ctx.restore();
}
function CgDrawTarget(cdata, ctx) {
  ctx.save();
  var ymin = cdata.data.min;
  var ymax = cdata.data.max;
  var ybest = cdata.data.best;
  ctx.fillStyle = "lightgray";
  ctx.beginPath();
  ctx.moveTo(cdata.bottomLeftX, CgAcxn2screen(cdata, ymin));
  ctx.lineTo(cdata.bottomRightX, CgAcxn2screen(cdata, ymin));
  ctx.lineTo(cdata.bottomRightX, CgAcxn2screen(cdata, ymax));
  ctx.lineTo(cdata.bottomLeftX, CgAcxn2screen(cdata, ymax));
  ctx.closePath();
  ctx.fill();
  ctx.beginPath();
  ctx.moveTo(cdata.bottomLeftX, CgAcxn2screen(cdata, ybest));
  ctx.lineTo(cdata.bottomRightX, CgAcxn2screen(cdata, ybest));
  ctx.stroke();
  ctx.textAlign = "left";
  ctx.textBaseline = "top";
  ctx.fillStyle = "black";
  ctx.fillText(
    cdata.data.textBestTarget,
    cdata.leftMargin + 3 * cdata.scale,
    CgAcxn2screen(cdata, ybest) + 3 * cdata.scale,
  );
  ctx.restore();
  ctx.save();
  ctx.textAlign = "center";
  ctx.textBaseline = "top";
  ctx.fillStyle = "black";
  ctx.translate(
    cdata.topRightX + 3 * cdata.scale,
    CgAcxn2screen(cdata, (ymax + ymin) / 2) + 3 * cdata.scale,
  );
  ctx.rotate(-Math.PI / 2);
  ctx.fillText(cdata.data.textTherapeuticRange, 0, 0);
  ctx.restore();
}
function CgDrawValues(cdata, ctx) {
  ctx.save();
  ctx.fillStyle = "black";
  ctx.lineWidth = 1 * cdata.scale;
  var x1 = cdata.bottomLeftX + (cdata.bottomRightX - cdata.bottomLeftX) * 0.25;
  var x2 = cdata.bottomLeftX + (cdata.bottomRightX - cdata.bottomLeftX) * 0.75;
  ctx.beginPath();
  ctx.arc(
    x1,
    CgAcxn2screen(cdata, cdata.data.valueBefore),
    5 * cdata.scale,
    0,
    2 * Math.PI,
    true,
  );
  ctx.fill();
  ctx.stroke();
  ctx.beginPath();
  ctx.arc(
    x2,
    CgAcxn2screen(cdata, cdata.data.valueAfter),
    5 * cdata.scale,
    0,
    2 * Math.PI,
    true,
  );
  ctx.fill();
  ctx.stroke();
  ctx.restore();
}
function CgUpdateChartDimensions(cdata, canvas) {
  cdata.topLeftX = cdata.leftMargin;
  cdata.topLeftY = cdata.topMargin;
  cdata.topRightX = canvas.width - cdata.rightMargin;
  cdata.topRightY = cdata.topMargin;
  cdata.bottomLeftX = cdata.leftMargin;
  cdata.bottomLeftY = canvas.height - cdata.bottomMargin;
  cdata.bottomRightX = canvas.width - cdata.rightMargin;
  cdata.bottomRightY = canvas.height - cdata.bottomMargin;
  cdata.plotWidth = cdata.topRightX - cdata.topLeftX;
  cdata.plotHeight = cdata.bottomLeftY - cdata.topLeftY;
}
function CgMaxYDisplayedValue(yFactor, minY, maxY) {
  var maxY2 = maxY * yFactor;
  var lg10 = Math.pow(10, Math.ceil(Math.log(1.1 * maxY2) / Math.LN10) - 1);
  var maxY3 = Math.ceil((1.1 / lg10) * maxY2) * lg10;
  return maxY3;
}
function CgDrawNonPlotArea(cdata, ctx) {
  var yFactor = 1.0;
  maxY = Math.max(
    cdata.data.max,
    cdata.data.valueBefore,
    cdata.data.valueAfter,
  );
  cdata.maxY = CgMaxYDisplayedValue(yFactor, cdata.minX, maxY);
  cdata.xRatio = cdata.plotWidth / (cdata.maxX - cdata.minX);
  cdata.yRatio = cdata.plotHeight / (cdata.maxY - cdata.minY);
  CgDrawTarget(cdata, ctx);
  CgDrawAxis(cdata, ctx);
  ctx.restore();
  ctx.save();
  CgDrawAxisTicks(cdata, ctx);
  CgDrawAxisLabels(cdata, ctx);
  CgDrawValues(cdata, ctx);
}
function CgDrawGraph(canvas, data) {
  CgUpdateChartDimensions(data, canvas);
  var ctx = canvas.getContext("2d");
  CgDrawNonPlotArea(data, ctx);
}
