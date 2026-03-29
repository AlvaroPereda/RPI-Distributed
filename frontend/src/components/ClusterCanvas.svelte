<script lang="ts">
    import Node from "./Node.svelte";
    import { deviceState } from "$lib/deviceState.svelte";

    const radius = 250;
    const size = 600;
    const center = size / 2;

    const getPositionStyle = (index: number, total: number) => {
        const angle = (2 * Math.PI * index) / total - (Math.PI / 2);

        const x = center + radius * Math.cos(angle);
        const y = center + radius * Math.sin(angle);

        return `
            position: absolute;
            left: ${x}px;
            top: ${y}px;
            transform: translate(-50%, -50%);
        `;
    }
</script>

<div class="circle-container">
    <div class="circle-area" style="width: {size}px; height: {size}px;">
        <div class="hub-center">
            <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                <circle cx="12" cy="12" r="3"/>
                <path d="M12 1v4M12 19v4M4.22 4.22l2.83 2.83M16.95 16.95l2.83 2.83M1 12h4M19 12h4M4.22 19.78l2.83-2.83M16.95 7.05l2.83-2.83"/>
            </svg>
        </div>

        {#each deviceState.current as device, index}
            <div class="node-wrapper" style={getPositionStyle(index, deviceState.current.length)}>
                <Node {device} />
            </div>
        {/each}
    </div>

    {#if deviceState.current.length === 0}
        <div class="empty-state">
            <p class="empty-title">No nodes connected</p>
            <p class="empty-sub">Add a device from the panel →</p>
        </div>
    {/if}
</div>

<style>
    .circle-container {
        display: flex;
        align-items: center;
        justify-content: center;
        width: 100%;
        height: 100%;
        position: relative;
    }

    .circle-area {
        position: relative;
        top: -60px;
    }

    /* Orbit ring */
    .circle-area::before {
        content: '';
        position: absolute;
        top: 50%;
        left: 50%;
        width: 500px;
        height: 500px;
        transform: translate(-50%, -50%);
        border-radius: 50%;
        border: 1px dashed rgba(74, 222, 128, 0.12);
        pointer-events: none;
    }

    /* Outer glow ring */
    .circle-area::after {
        content: '';
        position: absolute;
        top: 50%;
        left: 50%;
        width: 520px;
        height: 520px;
        transform: translate(-50%, -50%);
        border-radius: 50%;
        box-shadow: inset 0 0 60px rgba(74, 222, 128, 0.04);
        pointer-events: none;
    }

    /* Center hub */
    .hub-center {
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        width: 52px;
        height: 52px;
        border-radius: 50%;
        background-color: var(--bg-surface);
        border: 1px solid rgba(74, 222, 128, 0.3);
        box-shadow: 0 0 20px rgba(74, 222, 128, 0.15), inset 0 0 15px rgba(74, 222, 128, 0.05);
        display: flex;
        align-items: center;
        justify-content: center;
        color: var(--color-accent-green);
        z-index: 2;
    }

    .node-wrapper {
        z-index: 3;
    }

    /* Empty state */
    .empty-state {
        position: absolute;
        top: calc(50% + 20px);
        left: 50%;
        transform: translate(-50%, -50%);
        text-align: center;
        pointer-events: none;
    }

    .empty-title {
        margin: 0 0 4px;
        font-family: var(--font-sans);
        font-size: 14px;
        font-weight: 500;
        color: var(--text-muted);
    }

    .empty-sub {
        margin: 0;
        font-family: var(--font-sans);
        font-size: 12px;
        color: var(--text-muted);
        opacity: 0.6;
    }
</style>
