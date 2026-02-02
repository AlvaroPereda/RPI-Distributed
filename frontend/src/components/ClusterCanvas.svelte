<script lang="ts">
    import type { Device } from "$lib/types";
    import Node from "./Node.svelte";

    let { cluster }: { cluster: Device[] } = $props();

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
        {#each cluster as device, index}
            <div class="node-wrapper" style={getPositionStyle(index, cluster.length)}>
                <Node {device} />
            </div>
        {/each}
    </div>
</div>

<style>
    .circle-container {
        display: flex;
        align-items: center;
        justify-content: center;
        width: 100%;
        height: 100%;
    }
    .circle-area {
        position: relative;
        border-radius: 50%;
        top: -60px;
    }
</style>