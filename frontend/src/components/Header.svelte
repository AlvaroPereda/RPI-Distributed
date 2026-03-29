<script lang="ts">
    import { model } from "$lib/stores";
    import { goto } from "$app/navigation";

    const goBack = () => {
        goto("/")
    }
</script>

<header class="header">
    <button onclick={goBack} class="back-btn">
        <svg class="back-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <polyline points="15 18 9 12 15 6"></polyline>
        </svg>
        <span>Back</span>
    </button>

    <div class="model-badge">
        <span class="model-dot"></span>
        <span class="model-name">{$model}</span>
    </div>
</header>

<style>
    .header {
        display: flex;
        align-items: center;
        justify-content: center;
        background-color: var(--bg-deep);
        border-bottom: 1px solid var(--color-border);
        position: fixed;
        top: 0;
        left: 0;
        right: 0;
        height: 60px;
        z-index: 100;
        padding: 0 24px;
    }

    .back-btn {
        position: absolute;
        left: 24px;
        display: flex;
        align-items: center;
        gap: 6px;
        background: transparent;
        color: var(--text-secondary);
        border: 1px solid var(--color-border);
        border-radius: var(--border-radius-md);
        font-family: var(--font-sans);
        font-size: 13px;
        font-weight: 500;
        padding: 6px 12px;
        cursor: pointer;
        transition: color 0.2s ease, border-color 0.2s ease, background-color 0.2s ease;
    }

    .back-btn:hover {
        color: var(--text-primary);
        border-color: var(--text-muted);
        background-color: var(--bg-surface);
    }

    .back-icon {
        width: 14px;
        height: 14px;
        flex-shrink: 0;
    }

    .model-badge {
        display: flex;
        align-items: center;
        gap: 8px;
        background-color: var(--bg-surface);
        border: 1px solid var(--color-border);
        border-radius: 20px;
        padding: 5px 14px;
    }

    .model-dot {
        width: 7px;
        height: 7px;
        background-color: var(--color-accent-green);
        border-radius: 50%;
        box-shadow: var(--shadow-glow);
        animation: pulse 2.5s ease-in-out infinite;
    }

    .model-name {
        font-family: var(--font-mono);
        font-size: 12px;
        color: var(--text-secondary);
        letter-spacing: 0.03em;
        white-space: nowrap;
        overflow: hidden;
        text-overflow: ellipsis;
        max-width: 300px;
    }

    @keyframes pulse {
        0%, 100% { opacity: 1; }
        50% { opacity: 0.4; }
    }
</style>
