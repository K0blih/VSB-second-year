const userIdInput = document.getElementById("userId");
const loadFilesButton = document.getElementById("loadFilesButton");
const uploadForm = document.getElementById("uploadForm");
const fileInput = document.getElementById("fileInput");
const uploadStatus = document.getElementById("uploadStatus");
const listStatus = document.getElementById("listStatus");
const emptyState = document.getElementById("emptyState");
const fileList = document.getElementById("fileList");
const fileCount = document.getElementById("fileCount");

userIdInput.value = localStorage.getItem("object-storage-user-id") || "user-1";

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

function formatSize(size) {
  if (size < 1024) {
    return `${size} B`;
  }
  if (size < 1024 * 1024) {
    return `${(size / 1024).toFixed(1)} KB`;
  }
  return `${(size / (1024 * 1024)).toFixed(1)} MB`;
}

function renderFiles(files) {
  fileList.innerHTML = "";
  fileCount.textContent = `${files.length} file${files.length === 1 ? "" : "s"}`;
  emptyState.style.display = files.length ? "none" : "block";

  for (const file of files) {
    const card = document.createElement("article");
    card.className = "file-card";

    const createdAt = new Date(file.created_at).toLocaleString();
    card.innerHTML = `
      <div>
        <h3>${file.filename}</h3>
        <p class="meta">ID: <code>${file.id}</code></p>
        <p class="meta">Size: ${formatSize(file.size)} • Created: ${createdAt}</p>
      </div>
      <div class="file-actions">
        <button class="secondary" data-action="download" data-id="${file.id}">Download</button>
        <button class="danger" data-action="delete" data-id="${file.id}">Delete</button>
      </div>
    `;

    fileList.appendChild(card);
  }
}

async function apiFetch(path, options = {}) {
  const userId = getUserId();
  const response = await fetch(path, {
    ...options,
    headers: {
      "X-User-Id": userId,
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
      // Ignore JSON parse failures and keep default message.
    }
    throw new Error(message);
  }

  return response;
}

async function loadFiles() {
  setStatus(listStatus, "Loading files...");
  try {
    const response = await apiFetch("/files");
    const files = await response.json();
    renderFiles(files);
    setStatus(listStatus, files.length ? "Files loaded." : "No files stored for this user.");
  } catch (error) {
    renderFiles([]);
    setStatus(listStatus, error.message, true);
  }
}

async function handleUpload(event) {
  event.preventDefault();
  const selectedFile = fileInput.files[0];
  if (!selectedFile) {
    setStatus(uploadStatus, "Select a file first.", true);
    return;
  }

  const formData = new FormData();
  formData.append("file", selectedFile);

  setStatus(uploadStatus, "Uploading...");
  try {
    const response = await apiFetch("/files/upload", {
      method: "POST",
      body: formData,
    });
    const payload = await response.json();
    setStatus(uploadStatus, `Uploaded ${payload.filename} (${formatSize(payload.size)}).`);
    uploadForm.reset();
    await loadFiles();
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
  } catch (error) {
    setStatus(listStatus, error.message, true);
  }
}

async function deleteFile(fileId) {
  setStatus(listStatus, "Deleting file...");
  try {
    await apiFetch(`/files/${fileId}`, { method: "DELETE" });
    setStatus(listStatus, "File deleted.");
    await loadFiles();
  } catch (error) {
    setStatus(listStatus, error.message, true);
  }
}

loadFilesButton.addEventListener("click", loadFiles);
uploadForm.addEventListener("submit", handleUpload);
fileList.addEventListener("click", async (event) => {
  const button = event.target.closest("button[data-action]");
  if (!button) {
    return;
  }

  const { action, id } = button.dataset;
  if (action === "download") {
    await downloadFile(id);
  }
  if (action === "delete") {
    await deleteFile(id);
  }
});

loadFiles();
