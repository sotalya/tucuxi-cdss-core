// SPDX-License-Identifier: AGPL-3.0-or-later
/**
 * Parse various date inputs into a JS Date.
 * - Accepts Date, number (epoch ms), or string (ISO-ish, "YYYY-MM-DD HH:mm:ss", etc.)
 * - Trims milliseconds and trailing "Z", normalizes "T" to space
 * - Falls back to new Date(d) if parsing fails
 */
function makeDate(d) {
  if (d == null) return null;
  if (d instanceof Date) return d;
  if (typeof d === 'number') return new Date(d);

  if (typeof d !== 'string') d = String(d);

  d = d.trim()
       .replace('T', ' ')
       .replace('Z', '')
       .replace(/\.\d+$/, '');

  const [datePart, timePart = '00:00:00'] = d.split(/\s+/);
  const [Y, M, D] = (datePart || '').split(/[-/]/).map(Number);
  const [h = 0, m = 0, s = 0] = (timePart || '').split(':').map(Number);

  if ([Y, M, D].some(n => Number.isNaN(n))) return new Date(d);

  return new Date(Y, (M || 1) - 1, D || 1, h || 0, m || 0, s || 0);
}

/**
 * Map human-readable target types to internal numeric codes, then push GraphTarget.
 * @param {GraphFullData} d - target container
 * @param {Array<[string, number, number, number]>} c - targets as [type, min, best, max]
 */
function addTargets(d, c) {
  for (var b = 0; b < c.length; b++) {
    var a = -1;
    switch (c[b][0]) {
      case "residual":
        a = 0;
        break;
      case "peak":
        a = 1;
        break;
      case "mean":
        a = 2;
        break;
      case "auc":
        a = 3;
        break;
      case "auc24":
        a = 4;
        break;
      case "cumulativeAuc":
        a = 5;
        break;
      case "aucDividedByMic":
        a = 6;
        break;
      case "auc24DividedByMic":
        a = 7;
        break;
      case "aucOverMic":
        a = 8;
        break;
      case "auc24OverMic":
        a = 9;
        break;
      case "peakDividedByMic":
        a = 10;
        break;
      case "timeOverMic":
        a = 11;
        break;
      case "residualDividedByMic":
        a = 12;
    }
    d.targets.push(new GraphTarget(a, c[b][1], c[b][2], c[b][3]));
  }
}

/**
 * Append (times, values) into a GraphAdjustment predictionData,
 * computing absolute UNIX seconds from start date + hour offsets.
 * Also push the last index to troughs as a marker (legacy behavior).
 */
function addPredictionData(a, d, c, e) {
  for (var b = 0; b < c.length; b++)
    a.predictionData.time.push(3600 * c[b] + d.getTime() / 1e3),
      a.predictionData.value.push(e[b]);
  return a.predictionData.troughs.push(a.predictionData.value.length - 1), a;
}

/**
 * High-level orchestration: feed raw inputs into GraphFullData, then draw.
 * @param {GraphFullData} graphFullData - main graph model
 * @param {Array} adjData - adjustments grouped by cycles
 * @param {Date} adjStartDate - earliest date (already a Date)
 * @param {string|Date|number} adjEndDate - latest date (parsed by makeDate)
 * @param {Array} aprioriPercentiles - list of percentile objects (apriori)
 * @param {Array} aPosterioriPredicction - list of [start, times[], values[]] (posteriori)
 * @param {Array} aPrioriPrediction - list of [start, times[], values[]] (apriori)
 * @param {Array} aPosterioriPercentiles - list of percentile objects (posteriori)
 * @param {Array} samples - lab samples
 */
function displayGraphs(
  graphFullData,
  adjData = [],
  adjStartDate,
  adjEndDate,
  aprioriPercentiles = [],
  aPosterioriPredicction = [],
  aPrioriPrediction = [],
  aPosterioriPercentiles = [],
  samples = []
) {
  if (!graphFullData) {
    console.error("graphFullData is required.");
    return;
  }

  // Important: timestart is already a Date in the caller's pipeline
  graphFullData.timestart = adjStartDate;
  graphFullData.timeend   = makeDate(adjEndDate);

  // Mark blocks as valid by default (they may be refined by processors)
  graphFullData.revP.isValid  = true;
  graphFullData.apoP.isValid  = true;
  graphFullData.aprP.isValid  = true;
  graphFullData.scale         = 1;

  // Feed data into the graph model
  processSamples(graphFullData, samples);
  processAprioriPercentiles(graphFullData, aprioriPercentiles);
  processPosterioriPercentiles(graphFullData, aPosterioriPercentiles);
  processPredictionData(graphFullData, aPosterioriPredicction, 'apoP');
  processPredictionData(graphFullData, aPrioriPrediction, 'aprP');
  processAdjData(graphFullData, adjData);

  // Render
  drawGraph(graphFullData);
}

/**
 * Normalize and push lab samples into GraphFullData.measures.
 */
function processSamples(graphFullData, samples) {
  samples.forEach(sample => {
    if (!sample.measure || !sample.date) {
      console.warn("Invalid sample", sample);
      return;
    }
    
    let [measureValue, measureUnit] = sample.measure.split(" ");
    measureValue = parseFloat(measureValue);
    if (isNaN(measureValue)) {
      console.warn("Invalid measure value", sample.measure);
      return;
    }
     
    const [datePart, timePart] = sample.date.split(" ");
    const [day, month, year] = datePart.split(".").map(Number);
    const [hours, minutes] = timePart.replace("h", ":").split(":").map(Number);
    const jsDate = new Date(year, month - 1, day, hours, minutes);

    graphFullData.measures.push(
      new GraphMeasure(jsDate, measureValue, sample.percentile, measureUnit)
    );
  });
}

/**
 * Build apriori percentile bands by rank and append them into aprpercsP.
 * - Accepts times as string "1,2,3" or number[]
 * - Computes absolute UNIX seconds from start + hour offsets
 */
function processAprioriPercentiles(graphFullData, aprioriPercentiles) {
  const percentileMap = {};

  aprioriPercentiles.forEach(percentile => {
    if (!percentile.times || !percentile.values || percentile.rank == null || !percentile.start) {
      console.warn("Invalid apriori percentile", percentile);
      return;
    }

    const rank = Number(percentile.rank);
    if (![5, 10, 25, 50, 75, 90, 95].includes(rank)) {
      console.warn("Invalid percentile rank", percentile.rank);
      return;
    }

    const startTime = new Date(percentile.start.replace(" ", "T"));
    if (isNaN(startTime.getTime())) {
      console.warn("Invalid percentile start date", percentile.start);
      return;
    }

    const rawTimes = typeof percentile.times === "string" 
      ? percentile.times.split(',').map(Number) 
      : percentile.times;

    const values = typeof percentile.values === "string" 
      ? percentile.values.split(',').map(Number) 
      : percentile.values;

    const computedTimes = rawTimes.map(t => 3600 * t + startTime.getTime() / 1e3);

    if (!percentileMap[rank]) {
      percentileMap[rank] = {
        predictionData: new GraphPredictionData([], []),
        rank: rank
      };
    }

    percentileMap[rank].predictionData.time.push(...computedTimes);
    percentileMap[rank].predictionData.value.push(...values);
  });
  let appendedAny = false;

  for (const rank in percentileMap) {
    const entry = percentileMap[rank];

    entry.predictionData.isValid = true;

    const p = new GraphPercentileData();
    p.percentile = entry.rank;
    p.predictionData = entry.predictionData;
    p.isValid = true;
    appendedAny = true;
    graphFullData.aprpercsP.append(p);
  }

  graphFullData.aprpercsP.isValid = appendedAny === true;
  preparePercs(graphFullData.aprpercsP);
}


/**
 * Posteriori percentiles (structure differs from apriori feed).
 * NOTE: This pathway could be unified with apriori handling later.
 */
function processPosterioriPercentiles(graphFullData, aPosterioriPercentiles) {
  let appendedAny = false;

  aPosterioriPercentiles.forEach(percentile => {
    if (!percentile.times || !percentile.values || percentile.rank == null) {
      console.warn("Invalid aPosteriori percentile", percentile);
      return;
    }

    if (![5, 10, 25, 50, 75, 90, 95].includes(percentile.rank)) {
      console.warn("Invalid percentile rank", percentile.rank);
      return;
    }
    let data = new GraphPredictionData(percentile.times, percentile.values);
    data.isValid = true;
    
    let p = new GraphPercentileData();
    p.percentile = percentile.rank;
    p.predictionData = data;
    p.isValid = true;
    
    appendedAny = true;
    graphFullData.apopercsP.append(p);
    
  });

  graphFullData.apopercsP.isValid = appendedAny === true;;
  preparePercs(graphFullData.apopercsP);
}

/**
 * Build predictive curves:
 * - predictionData is a list of tuples: [startDateStr, times[], values[]]
 * - Computes absolute UNIX seconds from start + hour offsets
 * - Stores into graphFullData[key] (e.g., 'apoP' or 'aprP')
 */
function processPredictionData(graphFullData, predictionData, key) {
  if (!predictionData || !Array.isArray(predictionData) || predictionData.length === 0) {
    console.warn("Invalid prediction data", predictionData);
    return;
  }
  let graphPredictionResult = new GraphPredictionResult();
  let graphPredictive = new GraphPredictive();
  let totalPoints = 0;

  predictionData.forEach(predictions => {
    if (!Array.isArray(predictions) || predictions.length < 3) {
      console.warn("Malformed prediction tuple", predictions, key);
      return;
    }
    let [dateStr, xValues, yValues] = predictions;
    let date = new Date(dateStr.replace(' ', 'T'));

    if (isNaN(date.getTime())) {
      console.warn("Invalid date in prediction set", dateStr, key);
      return;
    }

    for (let i = 0; i < xValues.length; i++) {
      const x = Number(xValues[i]);
      const y = Number(yValues[i]);

      graphPredictive.predictionData.time.push(3600 * x + date.getTime() / 1e3);
      graphPredictive.predictionData.value.push(y);
      totalPoints++;
    }
  });

  if (totalPoints === 0) {
    console.warn("No valid prediction points added for", key);
    return;
  }


  graphPredictive.predictionData.isValid = true;
  graphPredictionResult.predictive = graphPredictive;
  graphFullData[key] = graphPredictionResult;

  preparePrediction(graphFullData[key]);
}

/**
 * Build reverse (suggested) adjustments:
 * - adjData is an array of groups; each group is a list of [start, times[], values[]]
 * - For the first group, update canvas references if #canBestAdj exists
 */
function processAdjData(graphFullData, adjData) {
  adjData.forEach((adjGroup, index) => {
    let e = new GraphAdjustment();
    
    adjGroup.forEach(adj => {
      if (!Array.isArray(adj) || adj.length < 3) {
        console.warn("Invalid adjustment data", adj);
        return;
      }
      e = addPredictionData(e, makeDate(adj[0]), adj[1], adj[2]);
    });
    
    e.predictionData.isValid = true;
    graphFullData.revP.append(e);
    
    if (index === 0) {
      let f = document.getElementById("canBestAdj");
      if (f) {
        graphFullData.updateChartDimensions(f);
        graphFullData.canvas = f;
        graphFullData.annotationsCanvas = f;
        graphFullData.clockCanvas = f;
      }
    }
  });
}
