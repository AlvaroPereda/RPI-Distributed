<script lang="ts">
    import type { Device } from "$lib/types";

    let { device }: { device: Device} = $props()
</script>

<div class="node-container">
    <div class="circle">
        <svg xmlns="http://www.w3.org/2000/svg" width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
            <rect width="16" height="16" x="4" y="4" rx="2"/>
            <rect width="6" height="6" x="9" y="9" rx="1"/>
            <path d="M15 2v2"/><path d="M15 20v2"/><path d="M2 15h2"/><path d="M2 9h2"/><path d="M20 15h2"/><path d="M20 9h2"/><path d="M9 2v2"/><path d="M9 20v2"/>
        </svg>
    </div>

    <div class="info">
        <span class="name">{device.user}</span>
        <span class="ip">{device.ip}</span>
    </div>
</div>

<style>
    .node-container {
        position: absolute;
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: 8px;
        width: max-content;
    }

    .circle {
        width: 72px;
        height: 72px;
        border-radius: 50%;
        background-color: var(--bg-surface-elevated);
        border: 1px solid var(--color-accent-green);
        display: flex;
        justify-content: center;
        align-items: center;
        color: var(--color-accent-green);
        transition: box-shadow 0.3s ease, border-color 0.3s ease, transform 0.2s ease;
        box-shadow: 0 0 12px rgba(74, 222, 128, 0.15), 0 4px 16px rgba(0, 0, 0, 0.6);
        z-index: 2;
    }

    .circle:hover {
        box-shadow: var(--shadow-glow), 0 4px 24px rgba(0, 0, 0, 0.7);
        border-color: #6ee8a0;
        transform: scale(1.06);
    }

    .info {
        display: flex;
        flex-direction: column;
        align-items: center;
        text-align: center;
        gap: 2px;
    }

    .name {
        font-family: var(--font-sans);
        font-size: 12px;
        font-weight: 600;
        color: var(--text-primary);
        letter-spacing: 0.01em;
    }

    .ip {
        font-family: var(--font-mono);
        font-size: 10px;
        color: var(--text-muted);
        letter-spacing: 0.03em;
    }
</style>
