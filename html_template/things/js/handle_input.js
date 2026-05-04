// SPDX-License-Identifier: AGPL-3.0-or-later
/* ===== Mini-helpers Vanilla (remplacent jQuery) ===== */
const $  = (sel, ctx = document) => ctx.querySelector(sel);
const $$ = (sel, ctx = document) => Array.from(ctx.querySelectorAll(sel));

const setHTML = (sel, html) => {
  const el = $(sel);
  if (el) el.innerHTML = html;
};

const hide = (sel) => {
  const el = $(sel);
  if (el) el.style.display = "none";
};

const show = (sel, d = "") => {
  const el = $(sel);
  if (el) el.style.display = d;
};

const onReady = (fn) => document.addEventListener("DOMContentLoaded", fn);

// Format YYYY-MM-DD
const fmtDate = (iso) => new Date(iso).toISOString().slice(0, 10);

async function fetchJSON() {
  try {
    const res = await fetch("data_result.json");
    if (!res.ok) throw new Error("Network response was not ok " + res.statusText);
    return await res.json();
  } catch (err) {
    console.error("fetchJSON error:", err);
    return null;
  }
}

/* =============================================================================
 * Parsing / normalisation
 * ============================================================================= */
function parseJsonMaybeString(fullJson) {
  return (typeof fullJson === "string") ? JSON.parse(fullJson) : fullJson;
}

function formatBirthdate(birthdate) {
  return (birthdate || "").split(" ")[0] || "N/A";
}

function capitalizeFirst(str) {
  if (!str) return "N/A";
  return str.charAt(0).toUpperCase() + str.slice(1);
}

/* =============================================================================
 * Section: Header / Contacts
 * ============================================================================= */
function renderHospital(data) {
  setHTML("#hospitalName",    data.contacts?.mandator?.institute?.name ?? "N/A");
  setHTML("#hospitalContact", data.contacts?.mandator?.institute?.phone ?? "N/A");
  setHTML(
    "#reportDate",
    data.header?.computation_time ? data.header.computation_time : "N/A"
  );
}

function renderPractitioner(data) {
  setHTML("#practitionerName",     data.contacts?.mandator?.name ?? "N/A");
  setHTML("#practitionerLocation", data.contacts?.mandator?.institute?.name ?? "N/A");
  setHTML("#practitionerEmail",    data.contacts?.mandator?.email ?? "N/A");
  setHTML("#practitionerPhone",    data.contacts?.mandator?.phone ?? "N/A");
}

function renderPatient(data) {
  setHTML("#patientId",      data.contacts?.patient?.id ?? "N/A");
  setHTML("#patientName",    data.contacts?.patient?.name ?? "N/A");
  setHTML("#patientAddress", data.contacts?.patient?.address ?? "N/A");
  setHTML("#patientCity",    "N/A");
  setHTML("#patientEmail",   data.contacts?.patient?.email ?? "N/A");
  setHTML("#patientPhone",   data.contacts?.patient?.phone ?? "N/A");
  setHTML("#patientDOB",     formatBirthdate(data.contacts?.patient?.birthdate ?? ""));
}

/* =============================================================================
 * Section: Drug name + translations
 * ============================================================================= */
function computeAndRenderDrugName(data) {
  const drugName = data.intro?.drug_id || "";
  const formattedDrugName = capitalizeFirst(drugName);

  $$(".drug_name").forEach((el) => { el.innerHTML = formattedDrugName; });
  return formattedDrugName;
}

function renderTranslations(data) {
  $$(".dosage_translate").forEach((el) => { el.innerHTML = data.adjustments?.dosage_translation ?? "N/A"; });

  setHTML("#hospital_translate",          data.report_translation?.hospital_translate ?? "N/A");
  setHTML("#report_date_translate",       data.report_translation?.report_date_translate ?? "N/A");
  setHTML("#requesting_practi_translate", data.report_translation?.requesting_practi_translate ?? "N/A");
  setHTML("#page_title",                  data.report_translation?.page_title ?? "N/A");

  $$(".name_translate").forEach((el) => { el.innerHTML = data.contacts?.name_translation ?? "N/A"; });
  $$(".phone_translate").forEach((el) => { el.innerHTML = data.contacts?.phone_translation ?? "N/A"; });
  $$(".address_translation").forEach((el) => { el.innerHTML = data.contacts?.address_translation ?? "N/A"; });
  $$(".email_translation").forEach((el) => { el.innerHTML = data.contacts?.email_translation ?? "N/A"; });
}

/* =============================================================================
 * Section: Covariates
 * ============================================================================= */
function renderCovariates(data) {
  const covariates = data.covariates?.covariates || [];
  const clinicalParams = data.computation_covariates?.computation_covariates || [];
  const clinicalParamNames = clinicalParams.map((p) => p.name);

  let warningsCovariate = [];

  if (covariates.length > 0) {
    let html = "";

    const clinicalCovariates = covariates.filter((c) => clinicalParamNames.includes(c.name));
    const otherCovariates = covariates.filter(
      (c) => (!clinicalParamNames.includes(c.name) && c.name !== "birthdate")
    );

    if (clinicalCovariates.length && otherCovariates.length) {
      html += `<tr class="section-header"><td colspan="2">Used Covariates</td></tr>`;
    }

    clinicalCovariates.forEach((c) => {
      const covariateValue = (c.value || "N/A").replace(" (patient)", "");
      const hasWarn = Boolean(c.warning);
      if (hasWarn) warningsCovariate.push({ name: c.name, warning: c.warning });

      html += `
          <tr class="highlight ${hasWarn ? "warning-row" : ""}">
            <td>${c.name || "N/A"} ${hasWarn ? '<span class="warning-icon">&#9888;</span>' : ""}</td>
            <td>${covariateValue}</td>
          </tr>`;
    });

    if (otherCovariates.length) {
      html += `<tr class="section-header"><td colspan="2">Unused Covariates</td></tr>`;
    }

    otherCovariates.forEach((c) => {
      const val = (c.value || "N/A").replace(" (patient)", "");
      const hasWarn = Boolean(c.warning);
      if (hasWarn) warningsCovariate.push({ name: c.name, warning: c.warning });

      html += `
        <tr class="${hasWarn ? "warning-row" : ""}">
          <td>${c.name || "N/A"} ${hasWarn ? '<span class="warning-icon">&#9888;</span>' : ""}</td>
          <td>${val}</td>
        </tr>`;
    });

    setHTML("#covariateDate", covariates[0]?.date ?? "N/A");
    setHTML("#covariatesBody", html);
  } else {
    setHTML("#covariatesBody", '<tr><td colspan="2">No covariates available</td></tr>');
  }

  if (warningsCovariate.length > 0) {
    let list = "";
    warningsCovariate.forEach((w) => { list += `<li><strong>${w.name}</strong>: ${w.warning}</li>`; });
    setHTML("#warningsCovariateList", list);
  } else {
    hide("#warningsCovariateContainer");
  }
}

/* =============================================================================
 * Section: PK Params
 * ============================================================================= */
function renderPkParams(data) {
  const pharmaParams = data.pks?.pks || [];
  if (pharmaParams.length) {
    let html = "";
    pharmaParams.forEach((p) => {
      html += `
          <tr>
            <td>${p.name || "N/A"}</td>
            <td>${p.typical_patient || "N/A"}</td>
            <td>${p.a_priori || "N/A"}</td>
          </tr>`;
    });
    setHTML("#pharmaParamsBody", html);
  } else {
    setHTML("#pharmaParamsBody", '<tr><td colspan="3">No clinical params available</td></tr>');
  }
}

/* =============================================================================
 * Section: Samples (blood concentrations)
 * ============================================================================= */
function renderSamples(data, formattedDrugName) {
  const samples = data.samples?.samples || [];
  let warningsSample = [];

  if (!samples.length) {
    setHTML("#sample_container", "");
    hide("#warningsSampleContainer");
    return;
  }

  let samplesHtml = `
      <div class="section">
        <div>
          <img class="img-logo" src="<!-- INLINE_IMG:assets/blood_concentration.txt -->" />
          <span class="title"><span>${formattedDrugName}</span> Blood Concentration</span>
        </div>
        <div id="samplesContainer">
          <table>
            <thead>
              <tr><th>Blood sample date (TAD<sup>1</sup>)</th><th>Plasma concentration</th></tr>
            </thead>
            <tbody id="samplesBody">`;

  samples.forEach((sample) => {
    const hasWarn = Boolean(sample.warning);
    if (hasWarn) warningsSample.push({ name: sample.date, warning: sample.warning });

    samplesHtml += `
        <tr class="${hasWarn ? "warning-row" : ""}">
          <td>${sample.date || "N/A"} (${sample.tad || "N/A"})</td>
          <td>${sample.measure || "N/A"} ${hasWarn ? '<span class="warning-icon">&#9888;</span>' : ""}</td>
        </tr>`;

    if (sample.warning_sentence) {
      const level = (sample.warning_level || "normal").toLowerCase();
      const inlineLevelClass =
        level === "warning" ? "level-warning" :
        level === "critical" ? "level-critical" : "";

      samplesHtml += `
          <tr class="sample-inline-warning">
            <td colspan="2">
              <div class="inline-box ${inlineLevelClass}">
                ${sample.warning_sentence}
              </div>
            </td>
          </tr>`;
    }
  });

  samplesHtml += `
            </tbody>
          </table>
        </div>
      </div>`;

  setHTML("#sample_container", samplesHtml);

  if (warningsSample.length > 0) {
    let list = "";
    warningsSample.forEach((w) => { list += `<li><strong>${w.name}</strong>: ${w.warning}</li>`; });
    setHTML("#warningsSampleList", list);
  } else {
    hide("#warningsSampleContainer");
  }
}

/* =============================================================================
 * Section: CDSS warnings/errors
 * ============================================================================= */
function renderCdssWarnings(data) {
  const errorCdss = data?.error_message || [];

  if (!errorCdss.length) {
    hide("#warningsCDSSContainer");
    return;
  }

  console.log("errorCdss", errorCdss);

  const errorCdssHtml = `
      <div class="section">
          <span class="warning-icon">&#9888;</span>${errorCdss}
        </div>
          `;

  setHTML("#warningsCDSS", errorCdssHtml);
}

/* =============================================================================
 * Section: Current Dosage
 * ============================================================================= */
function renderCurrentDosage(data, formattedDrugName) {
  const ranges = data.treatment?.dosage_time_ranges || [];
  const hasDoses = ranges[0]?.single_doses?.length > 0;

  if (!ranges.length || !hasDoses) {
    setHTML("#current_dosage_container", "");
    return;
  }

  const dosage = ranges[0].single_doses[0]?.dosage ?? "N/A";
  const startDate = ranges[0]?.date_from ?? "N/A";
  const lastDose = Array.isArray(data.intro?.last_dose)
    ? (data.intro.last_dose[0]?.date ?? "N/A")
    : "N/A";

  const html = `
      <div class="section">
        <div>
          <img class="img-logo" src="<!-- INLINE_IMG:assets/dosage.txt -->" />
          <span class="title">Current <span>${formattedDrugName}</span> Dosage</span>
        </div>
        <table><tbody>
          <tr><td><span>${formattedDrugName}</span> <span>${data.adjustments?.dosage_translation ?? ""}</span></td><td><span>${dosage}</span></td></tr>
          <tr><td>Start date</td><td><span>${startDate}</span></td></tr>
          <tr><td>Last dose taken</td><td><span>${lastDose}</span></td></tr>
        </tbody></table>
      </div>`;

  setHTML("#current_dosage_container", html);
}

/* =============================================================================
 * Section: Exposure + Recommendation + Graph placeholders in HTML
 * ============================================================================= */
function renderExposureAndRecommendation(data, formattedDrugName) {
  setHTML("#dailyExposure",     data?.compgraph_data?.compgraph_data?.[0]?.valueBefore ?? "N/A");
  setHTML("#therapeuticTarget", data?.targets?.targets?.[0]?.value ?? "N/A");

  const justificationInfo = data?.report_translation?.justification_info?.[0];
  let dosageHtml = "";

  if (justificationInfo) {
    const arrowMap = {
      increase: "<!-- INLINE_IMG:assets/increase_logo.txt -->",
      decrease: "<!-- INLINE_IMG:assets/decrease_logo.txt -->",
      equal: "<!-- INLINE_IMG:assets/equal_logo.txt -->",
      new: ""
    };
    const arrow = arrowMap[justificationInfo.justification_sign] || "";

    dosageHtml += `
        <div class="section red-border">
          <div class="title red-line-title">Dosage Recommendation</div>

          ${justificationInfo.type === "double" ? `
          <div class="dosage-block">
            <div class="dosage-icon">
              <img src="<!-- INLINE_IMG:assets/warning_logo.txt -->" alt="Warning Icon" class="icon"/>
            </div>
            <div class="dosage-info">
              <p class="dosage-before">${justificationInfo.before_title}</p>
              <p class="dosage-details">${justificationInfo.before_dosage_sentence}</p>
            </div>
          </div>` : ""}

          <div class="dosage-block">
            <div class="dosage-icon">
              ${arrow ? `<img src="${arrow}" alt="Dosage Icon" class="icon"/>` : ""}
            </div>
            <div class="dosage-info">
              <p class="dosage-new">${justificationInfo.new_title}</p>
              <p class="dosage-details">${justificationInfo.dosage_sentence}</p>
            </div>
          </div>

          <div class="justification-block">
            <div class="sampling-info">
              <p class="dosage-new">Justification:</p>
              <p class="dosage-details">
                ${justificationInfo.justification_exposure_sentence}
                ${justificationInfo.justification_dosage_sentence}
                ${justificationInfo.justification_interval_sentence}
              </p>
            </div>
          </div>
        </div>`;
  }

  const content = document.getElementById("adjustmentContent");
  const valueBefore = data?.compgraph_data?.compgraph_data?.[0]?.valueBefore;
  const formattedValueBefore =
    (typeof valueBefore === "number") ? valueBefore.toFixed(2) : (valueBefore ?? "N/A");

  const regex = /Min:\s*(\d+).*Max:\s*(\d+)/;
  const matches = (data?.targets?.targets?.[0]?.bounds || "").match(regex);

  const renderDefaultHTML = () => {
    content.innerHTML = `
        <div class="section">
          <div>
            <img class="img-logo" src="<!-- INLINE_IMG:assets/drug_exposure.txt -->" />
            <span class="title">Drug Exposure Level</span>
          </div>
          <table><tbody>
            <tr><td>Residual concentration</td><td><span id="dailyExposure">${formattedValueBefore} ug/l</span></td></tr>
            <tr><td>Target</td><td>${data.targets.targets[0].value}, [${parseInt(matches?.[1] || "0", 10)},${parseInt(matches?.[2] || "0", 10)}]</td></tr>
          </tbody></table>
        </div>

        <div>${dosageHtml}</div>

        <div class="section">
          <div class="title">Drug exposure</div>
          <div id="graphContainer"></div>
          <div class="title">Drug blood concentration over time</div>
          <div class='canvasAdjustments'>
            <canvas id='canBestAdj' width='716' height='474'></canvas>
          </div>
        </div>`;
  };

  const renderNoDataHTML = (message) => {
    content.innerHTML = `<div class="section"><h2>${message}</h2></div>`;
  };

  if (data?.adjustments?.no_adjustment_phrase) {
    renderNoDataHTML(data.adjustments.no_adjustment_phrase);
  } else {
    renderDefaultHTML();
  }

  // N/A info
  setHTML("#therapeuticRange", "N/A");
  setHTML("#therapeuticTargetInfo", "N/A");
}

/* =============================================================================
 * Graph logic
 * ============================================================================= */
function buildAdjustmentsData(jsonData) {
  return jsonData?.graph_data?.adjustments?.map((adjustment) => {
    return adjustment.cycles.map((cycle) => {
      const times_list = cycle.times.split(",").map((i) => parseFloat(i));
      const values_list = cycle.values.split(",").map((i) => parseFloat(i));
      return [cycle.start, times_list, values_list];
    });
  }) || [];
}

function buildTargets(jsonData) {
  return jsonData?.graph_data?.targets?.map((target) => {
    return [target.type, parseFloat(target.min), parseFloat(target.best), parseFloat(target.max)];
  }) || [];
}

function buildPosterioriPrediction(jsonData) {
  return jsonData?.pda_aposteriori_data?.concentrations?.map((c) => {
    return [c.start, c.times.split(",").map(Number), c.values.split(",").map(Number)];
  }) || [];
}

function computeEarliestDate(jsonData) {
  const dates = [];

  if (jsonData.graph_data?.adjustment_date) {
    dates.push(new Date(jsonData.graph_data.adjustment_date.replace(/['"]/g, "")));
  }

  jsonData.pda_aposteriori_data?.concentrations?.forEach((c) => { if (c.start) dates.push(new Date(c.start)); });
  jsonData.pda_apriori_data?.percentiles?.forEach((p) => { if (p.start) dates.push(new Date(p.start)); });

  return dates.reduce((min, current) => (current < min ? current : min));
}

function renderComparativeGraphs(dataList) {
  const graphContainer = document.getElementById("graphContainer");
  if (!graphContainer) {
    console.log("Comparative graphic container does not exist in HTML!");
    return;
  }

  graphContainer.innerHTML = "";
  dataList.forEach((dataCompGraph, index) => {
    const canvas = document.createElement("canvas");
    canvas.id = `graphCanvas${index}`;
    canvas.width = 600;
    canvas.height = 400;

    const canvasWrapper = document.createElement("div");
    canvasWrapper.className = "canvasAdjustments";
    canvasWrapper.appendChild(canvas);
    graphContainer.appendChild(canvasWrapper);

    const payload = {
      data: { ...dataCompGraph },
      scale: 1,
      leftMargin: 50,
      rightMargin: 50,
      topMargin: 50,
      bottomMargin: 50,
      minX: 0,
      maxX: 600,
      minY: 0,
      maxY: 170,
      fontSize: "10px",
      police: "sans-serif"
    };

    // Fonction existante (graphcreator.js / graphing.js)
    CgDrawGraph(canvas, payload);
  });
}

function renderMainGraphs(jsonData) {
  const adjustmentsData = buildAdjustmentsData(jsonData);
  const targets = buildTargets(jsonData);

  if (!(adjustmentsData.length > 0 && targets.length > 0)) {
    console.log("Aucune donnée disponible pour afficher les graphiques ou les cibles.");
    return;
  }

  const obj = new GraphFullData();
  if (!obj) {
    console.error("GraphAdjustment initialization failed");
    return;
  }

  const pda_aposteriori_Prediction = buildPosterioriPrediction(jsonData);

  addTargets(obj, targets);

  const earliestDate = computeEarliestDate(jsonData);

  displayGraphs(
    obj,
    adjustmentsData,
    earliestDate,
    jsonData.graph_data.end_date.replace(/['"]/g, ""),
    jsonData?.pda_apriori_data?.percentiles,
    pda_aposteriori_Prediction, // A Posteriori Prediction
    pda_aposteriori_Prediction, // A Priori Prediction
    [],                         // A Posteriori Percentiles
    jsonData.samples?.samples || []
  );
}

/* =============================================================================
 * Rendu principal
 * ============================================================================= */
function updateFieldsFromJson(fullJson) {
  const data = parseJsonMaybeString(fullJson);

  renderHospital(data);
  renderPractitioner(data);
  renderPatient(data);

  const formattedDrugName = computeAndRenderDrugName(data);

  renderTranslations(data);
  renderCovariates(data);
  renderPkParams(data);
  renderSamples(data, formattedDrugName);
  renderCdssWarnings(data);
  renderCurrentDosage(data, formattedDrugName);
  renderExposureAndRecommendation(data, formattedDrugName);
}

/* =============================================================================
 * Boot
 * ============================================================================= */
onReady(() => {
  const jsonScript = document.getElementById("jsonData")?.textContent || null;
  if (!jsonScript) return;

  const jsonData = JSON.parse(jsonScript);

  if (jsonData) {
    updateFieldsFromJson(jsonScript);

    renderMainGraphs(jsonData);

    if (jsonData?.compgraph_data?.compgraph_data?.length > 0) {
      renderComparativeGraphs(jsonData.compgraph_data.compgraph_data);
    }
  }
});