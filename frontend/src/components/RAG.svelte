<script lang="ts">
    import { notificationStore } from "$lib/notification.svelte";
    import { onMount } from "svelte";

    let documents: string[] = []
    let uploading = false
    let fileInput: HTMLInputElement

    const loadDocuments = async() => {
        const response = await fetch("/documents")
        if (response.ok) documents = await response.json()
    }

    const uploadFile = async(event: Event) => {
        try {
            const file = (event.target as HTMLInputElement).files?.[0]
            if (!file) return
            uploading = true
            const form = new FormData()
            form.append("file", file)

            const response =  await fetch("/document", { 
                method: "POST",
                body: form 
            })

            if(!response.ok) {
                const errorData = await response.json()
                throw new Error(errorData.error)
            }

            await loadDocuments()
            uploading = false
            fileInput.value = ""
        } catch (error) {
            notificationStore.add("error", error instanceof Error ? error.message : "Failed to upload file")
        }
    }

    const deleteDocument = async(document_name: string) => {
        const response = await fetch("/document", {
            method: "DELETE",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ document_name })
        })
        if (response.ok) await loadDocuments()
        else {
            const errorData = await response.json()
            notificationStore.add("error", errorData.error || "Failed to delete document")
        }
    }

    onMount(loadDocuments)
</script>

{#if documents.length > 0}
    <ul class="doc-list">
        {#each documents as doc}
            <li class="doc-item">
                <svg class="doc-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path>
                    <polyline points="14 2 14 8 20 8"></polyline>
                </svg>
                <span class="doc-name">{doc}</span>
                <button class="doc-delete" onclick={() => deleteDocument(doc)} aria-label="Delete">×</button>
            </li>
        {/each}
    </ul>
{:else}
    <p class="doc-empty">No documents indexed</p>
{/if}

<input
    bind:this={fileInput}
    type="file"
    accept=".txt,.md"
    style="display:none"
    onchange={uploadFile}
/>
<button class="upload-btn" onclick={() => fileInput.click()} disabled={uploading}>
    {#if uploading}
        Indexing…
    {:else}
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" width="13" height="13">
            <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
            <polyline points="17 8 12 3 7 8"></polyline>
            <line x1="12" y1="3" x2="12" y2="15"></line>
        </svg>
        Upload file
    {/if}
</button>

<style>
    .doc-list {
        list-style: none;
        margin: 0;
        padding: 0;
        display: flex;
        flex-direction: column;
        gap: 6px;
    }

    .doc-item {
        display: flex;
        align-items: center;
        gap: 8px;
        background-color: var(--bg-surface-elevated);
        border: 1px solid var(--color-border);
        border-radius: var(--border-radius-md);
        padding: 8px 10px;
    }

    .doc-icon {
        width: 13px;
        height: 13px;
        color: var(--text-muted);
        flex-shrink: 0;
    }

    .doc-name {
        font-family: var(--font-mono);
        font-size: 10px;
        color: var(--text-primary);
        overflow: hidden;
        text-overflow: ellipsis;
        white-space: nowrap;
        flex: 1;
    }

    .doc-delete {
        background: none;
        border: none;
        color: var(--text-muted);
        cursor: pointer;
        font-size: 15px;
        line-height: 1;
        padding: 0 2px;
        flex-shrink: 0;
        transition: color 0.15s ease;
    }

    .doc-delete:hover {
        color: #f87171;
    }

    .doc-empty {
        font-family: var(--font-sans);
        font-size: 12px;
        color: var(--text-muted);
        margin: 0;
        font-style: italic;
    }

    .upload-btn {
        display: flex;
        align-items: center;
        justify-content: center;
        gap: 7px;
        width: 100%;
        background-color: var(--bg-surface-elevated);
        border: 1px solid var(--color-border);
        color: var(--text-primary);
        font-family: var(--font-sans);
        font-size: 12px;
        font-weight: 500;
        padding: 9px 14px;
        border-radius: var(--border-radius-md);
        cursor: pointer;
        transition: border-color 0.2s ease, color 0.2s ease;
    }

    .upload-btn:hover:not(:disabled) {
        border-color: var(--text-muted);
        color: var(--color-accent-green);
    }

    .upload-btn:disabled {
        opacity: 0.5;
        cursor: not-allowed;
    }
</style>