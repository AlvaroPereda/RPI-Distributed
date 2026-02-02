<script lang="ts">
    import type { Device } from "$lib/types";

    let { device }: { device: Device} = $props()
</script>

<div class="node-container">
    <div class="circle">
        <svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <rect width="16" height="16" x="4" y="4" rx="2"/>
            <rect width="6" height="6" x="9" y="9" rx="1"/>
            <path d="M15 2v2"/><path d="M15 20v2"/><path d="M2 15h2"/><path d="M2 9h2"/><path d="M20 15h2"/><path d="M20 9h2"/><path d="M9 2v2"/><path d="M9 20v2"/>
        </svg>
    </div>

    <div class="info">
        <span class="name">{device.name}</span>
        <span class="ip">({device.ip})</span>
    </div>
</div>

<style>
    .node-container {
        position: absolute;
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: 0.5rem;
        width: max-content;
        font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace;
    }
    .circle {
        width: 80px;
        height: 80px;
        border-radius: 50%;
        border: 2px solid #6b7280;
        background-color: rgb(62, 62, 62);
        display: flex;
        justify-content: center;
        align-items: center;
        color: #9ca3af;
        transition: all 0.3s ease;
        box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.5);
        z-index: 2;
    }
    .circle:hover {
        border-color: #939393;
        color: #ffff;
        box-shadow: 0 0 15px rgba(131, 131, 131, 0.5);
    }
    .info {
        display: flex;
        flex-direction: column;
        align-items: center;
        text-align: center;
        line-height: 1.2;
    }
    .name {
        color: #e5e7eb;
        font-weight: 600;
        font-size: 0.9rem;
    }
    .ip {
        color: #9ca3af;
        font-size: 0.8rem;
    }
</style>