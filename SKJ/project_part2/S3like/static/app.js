const userIdInput = document.getElementById("userId");
const internalSourceCheckbox = document.getElementById("internalSource");
const loadBucketsButton = document.getElementById("loadBucketsButton");
const bucketForm = document.getElementById("bucketForm");
const bucketNameInput = document.getElementById("bucketNameInput");
const bucketStatus = document.getElementById("bucketStatus");
const bucketSelect = document.getElementById("bucketSelect");
const activeBucketBadge = document.getElementById("activeBucketBadge");
const uploadForm = document.getElementById("uploadForm");
const fileInput = document.getElementById("fileInput");
const uploadStatus = document.getElementById("uploadStatus");
const compactForm = document.getElementById("compactForm");
const compactVolumeInput = document.getElementById("compactVolumeInput");
const compactStatus = document.getElementById("compactStatus");
const billingStatus = document.getElementById("billingStatus");
const billingGrid = document.getElementById("billingGrid");
const listStatus = document.getElementById("listStatus");
const emptyState = document.getElementById("emptyState");
const fileList = document.getElementById("fileList");
const fileCount = document.getElementById("fileCount");

userIdInput.value = localStorage.getItem("object-storage-user-id") || "user-1";

let buckets = [];

function setStatus(element, message, isError = false) {
  element.textContent = message;
  element.classList.toggle("error", isError);
}

function getUserId() {
  const userId = userIdInput.value.trim();
  if (!userId) {
    throw new Error("Enter a user ID first.");
  }

  localStorage.setItem("object-storage-user-id", userId);
  return userId;
}

function getSelectedBucketId() {
  const bucketId = Number(bucketSelect.value);
  if (!bucketId) {
    throw new Error("Create or select a bucket first.");
  }
  return bucketId;
}

function formatSize(size) {
  if (size < 1024) {
    return `${size} B`;
  }
  if (size < 1024 * 1024) {
    return `${(size / 1024).toFixed(1)} KB`;
  }
  return `${(size / (1024 * 1024)).toFixed(1)} MB`;
}

function setActiveBucketBadge() {
  const bucket = buckets.find((item) => item.id === Number(bucketSelect.value));
  activeBucketBadge.textContent = bucket ? bucket.name : "No bucket selected";
}

function renderBuckets(bucketItems) {
  buckets = bucketItems;
  bucketSelect.innerHTML = "";

  if (!bucketItems.length) {
    const option = document.createElement("option");
    option.value = "";
    option.textContent = "No buckets available";
    bucketSelect.appendChild(option);
    bucketSelect.disabled = true;
    setActiveBucketBadge();
    return;
  }

  bucketSelect.disabled = false;
  for (const bucket of bucketItems) {
    const option = document.createElement("option");
    option.value = String(bucket.id);
    option.textContent = `${bucket.name} (#${bucket.id})`;
    bucketSelect.appendChild(option);
  }

  const storedBucketId = Number(localStorage.getItem("object-storage-bucket-id"));
  const matchingBucket = bucketItems.find((bucket) => bucket.id === storedBucketId);
  if (matchingBucket) {
    bucketSelect.value = String(matchingBucket.id);
  }

  setActiveBucketBadge();
}

function renderFiles(files) {
  fileList.innerHTML = "";
  fileCount.textContent = `${files.length} object${files.length === 1 ? "" : "s"}`;
  emptyState.style.display = files.length ? "none" : "block";

  for (const file of files) {
    const card = document.createElement("article");
    card.className = "file-card";
    card.dataset.fileId = file.id;

    const createdAt = new Date(file.created_at).toLocaleString();
    const location =
      file.volume_id && file.offset !== null && file.offset !== undefined
        ? `Volume: #${file.volume_id} • Offset: ${file.offset}`
        : "Volume: pending";
    card.innerHTML = `
      <div>
        <h3>${file.filename}</h3>
        <p class="meta">ID: <code>${file.id}</code></p>
        <p class="meta">Bucket: #${file.bucket_id} • Size: ${formatSize(file.size)} • Status: ${file.status} • Created: ${createdAt}</p>
        <p class="meta">${location}</p>
      </div>
      <div class="file-actions">
        <label class="compact-field">
          <span>Operation</span>
          <select data-role="operation">
            <option value="grayscale">Grayscale</option>
            <option value="negative">Negative</option>
            <option value="mirror">Mirror</option>
            <option value="brightness">Brightness</option>
            <option value="crop">Crop</option>
          </select>
        </label>
        <label class="compact-field">
          <span>Amount</span>
          <input data-role="brightness" type="number" value="50" />
        </label>
        <label class="compact-field">
          <span>Crop x/y/w/h</span>
          <input data-role="crop" type="text" placeholder="0,0,200,200" />
        </label>
        <button data-action="process" data-id="${file.id}">Process</button>
        <button class="secondary" data-action="download" data-id="${file.id}">Download</button>
        <button class="danger" data-action="delete" data-id="${file.id}">Soft Delete</button>
      </div>
    `;

    fileList.appendChild(card);
  }
}

function renderBilling(billing) {
  const items = [
    ["Storage", formatSize(billing.current_storage_bytes)],
    ["Bandwidth", formatSize(billing.bandwidth_bytes)],
    ["Ingress", formatSize(billing.ingress_bytes)],
    ["Egress", formatSize(billing.egress_bytes)],
    ["Internal", formatSize(billing.internal_transfer_bytes)],
    ["Write Requests", String(billing.count_write_requests)],
    ["Read Requests", String(billing.count_read_requests)],
  ];

  billingGrid.innerHTML = items
    .map(
      ([label, value]) => `
        <div class="billing-item">
          <dt>${label}</dt>
          <dd>${value}</dd>
        </div>
      `,
    )
    .join("");
}

async function apiFetch(path, options = {}) {
  const userId = getUserId();
  const response = await fetch(path, {
    ...options,
    headers: {
      "X-User-Id": userId,
      "X-Internal-Source": internalSourceCheckbox.checked ? "true" : "false",
      ...(options.headers || {}),
    },
  });

  if (!response.ok) {
    let message = `Request failed with status ${response.status}.`;
    try {
      const payload = await response.json();
      if (payload.detail) {
        message = payload.detail;
      }
    } catch {
      // Keep the default message for non-JSON error bodies.
    }
    throw new Error(message);
  }

  return response;
}

async function loadBucketObjects() {
  const bucketId = getSelectedBucketId();
  setStatus(listStatus, "Loading bucket objects...");
  try {
    const response = await apiFetch(`/buckets/${bucketId}/objects/`);
    const payload = await response.json();
    renderFiles(payload);
    setStatus(
      listStatus,
      payload.length ? "Bucket objects loaded." : "No active objects in this bucket.",
    );
  } catch (error) {
    renderFiles([]);
    setStatus(listStatus, error.message, true);
  }
}

async function loadBilling() {
  const bucketId = getSelectedBucketId();
  setStatus(billingStatus, "Loading billing...");
  try {
    const response = await apiFetch(`/buckets/${bucketId}/billing/`);
    const payload = await response.json();
    renderBilling(payload);
    setStatus(billingStatus, "Billing loaded.");
  } catch (error) {
    billingGrid.innerHTML = "";
    setStatus(billingStatus, error.message, true);
  }
}

async function refreshSelectedBucketData() {
  setActiveBucketBadge();
  localStorage.setItem("object-storage-bucket-id", bucketSelect.value || "");

  if (!bucketSelect.value) {
    renderFiles([]);
    billingGrid.innerHTML = "";
    setStatus(listStatus, "Select a bucket to inspect objects.");
    setStatus(billingStatus, "Select a bucket to inspect billing.");
    return;
  }

  await Promise.all([loadBucketObjects(), loadBilling()]);
}

async function loadBuckets() {
  setStatus(bucketStatus, "Loading buckets...");
  try {
    const response = await apiFetch("/buckets/");
    const payload = await response.json();
    renderBuckets(payload);
    setStatus(
      bucketStatus,
      payload.length ? "Buckets loaded." : "No buckets created for this user.",
    );
    await refreshSelectedBucketData();
  } catch (error) {
    renderBuckets([]);
    renderFiles([]);
    billingGrid.innerHTML = "";
    setStatus(bucketStatus, error.message, true);
    setStatus(listStatus, error.message, true);
    setStatus(billingStatus, error.message, true);
  }
}

async function handleCreateBucket(event) {
  event.preventDefault();

  const bucketName = bucketNameInput.value.trim();
  if (!bucketName) {
    setStatus(bucketStatus, "Enter a bucket name first.", true);
    return;
  }

  setStatus(bucketStatus, "Creating bucket...");
  try {
    await apiFetch("/buckets/", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ name: bucketName }),
    });
    bucketForm.reset();
    setStatus(bucketStatus, `Bucket ${bucketName} created.`);
    await loadBuckets();
  } catch (error) {
    setStatus(bucketStatus, error.message, true);
  }
}

async function handleUpload(event) {
  event.preventDefault();

  const selectedFile = fileInput.files[0];
  if (!selectedFile) {
    setStatus(uploadStatus, "Select a file first.", true);
    return;
  }

  let bucketId;
  try {
    bucketId = getSelectedBucketId();
  } catch (error) {
    setStatus(uploadStatus, error.message, true);
    return;
  }

  const formData = new FormData();
  formData.append("file", selectedFile);
  formData.append("bucket_id", String(bucketId));

  setStatus(uploadStatus, "Uploading object...");
  try {
    const response = await apiFetch("/files/upload", {
      method: "POST",
      body: formData,
    });
    const payload = await response.json();
    setStatus(uploadStatus, `Accepted ${payload.filename} (${formatSize(payload.size)}), finalizing storage.`);
    uploadForm.reset();
    await refreshSelectedBucketData();
  } catch (error) {
    setStatus(uploadStatus, error.message, true);
  }
}

async function downloadFile(fileId) {
  setStatus(listStatus, "Preparing download...");
  try {
    const response = await apiFetch(`/files/${fileId}`);
    const blob = await response.blob();
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");
    const disposition = response.headers.get("content-disposition") || "";
    const match = disposition.match(/filename="?([^"]+)"?/);
    link.href = url;
    link.download = match ? match[1] : fileId;
    document.body.appendChild(link);
    link.click();
    link.remove();
    URL.revokeObjectURL(url);
    setStatus(listStatus, "Download started.");
    await loadBilling();
  } catch (error) {
    setStatus(listStatus, error.message, true);
  }
}

async function deleteFile(fileId) {
  setStatus(listStatus, "Soft deleting object...");
  try {
    await apiFetch(`/files/${fileId}`, { method: "DELETE" });
    setStatus(listStatus, "Object soft deleted.");
    await refreshSelectedBucketData();
  } catch (error) {
    setStatus(listStatus, error.message, true);
  }
}

function processParamsFor(card, operation) {
  if (operation === "brightness") {
    const amount = Number(card.querySelector("[data-role='brightness']").value || 50);
    return { amount };
  }
  if (operation === "crop") {
    const raw = card.querySelector("[data-role='crop']").value.trim();
    if (!raw) {
      return { border: 100 };
    }
    const values = raw.split(",").map((item) => Number(item.trim()));
    if (values.length !== 4 || values.some((value) => !Number.isInteger(value))) {
      throw new Error("Crop must use x,y,width,height integer values.");
    }
    const [x, y, width, height] = values;
    return { x, y, width, height };
  }
  return {};
}

async function processFile(fileId, card) {
  const bucketId = getSelectedBucketId();
  const operation = card.querySelector("[data-role='operation']").value;
  let params;
  try {
    params = processParamsFor(card, operation);
  } catch (error) {
    setStatus(listStatus, error.message, true);
    return;
  }

  setStatus(listStatus, "Starting image processing...");
  try {
    await apiFetch(`/buckets/${bucketId}/objects/${fileId}/process`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ operation, params }),
    });
    setStatus(listStatus, `Processing job started for ${operation}.`);
    await loadBilling();
    window.setTimeout(loadBucketObjects, 1000);
  } catch (error) {
    setStatus(listStatus, error.message, true);
  }
}

async function handleCompactVolume(event) {
  event.preventDefault();

  const volumeId = Number(compactVolumeInput.value);
  if (!Number.isInteger(volumeId) || volumeId <= 0) {
    setStatus(compactStatus, "Enter a valid volume ID.", true);
    return;
  }

  setStatus(compactStatus, `Compacting volume ${volumeId}...`);
  try {
    const response = await apiFetch(`/admin/volumes/${volumeId}/compact`, {
      method: "POST",
      headers: { "X-Internal-Source": "true" },
    });
    const payload = await response.json();
    const savedBytes = Math.max(0, (payload.original_size || 0) - (payload.compacted_size || 0));
    setStatus(
      compactStatus,
      payload.rotated_active_volume
        ? `Volume ${volumeId} compacted: ${payload.objects_rewritten} live object(s) rewritten, ${formatSize(savedBytes)} freed. New uploads moved to the next volume.`
        : `Volume ${volumeId} compacted: ${payload.objects_rewritten} live object(s) rewritten, ${formatSize(savedBytes)} freed.`,
    );
    await refreshSelectedBucketData();
  } catch (error) {
    setStatus(compactStatus, error.message, true);
  }
}

loadBucketsButton.addEventListener("click", loadBuckets);
bucketForm.addEventListener("submit", handleCreateBucket);
bucketSelect.addEventListener("change", refreshSelectedBucketData);
uploadForm.addEventListener("submit", handleUpload);
compactForm.addEventListener("submit", handleCompactVolume);
fileList.addEventListener("click", async (event) => {
  const button = event.target.closest("button[data-action]");
  if (!button) {
    return;
  }

  const { action, id } = button.dataset;
  const card = button.closest(".file-card");
  if (action === "process") {
    await processFile(id, card);
  }
  if (action === "download") {
    await downloadFile(id);
  }
  if (action === "delete") {
    await deleteFile(id);
  }
});

loadBuckets();
