<script lang="ts">
    import type { Device } from "$lib/types";
    import Node from "./Node.svelte";

    let { cluster }: { cluster: Device[]}  = $props()
    
</script>
<div>
    <div>
        {#each cluster as device}
            <Node {device}/>
        {/each}
    </div>
</div>