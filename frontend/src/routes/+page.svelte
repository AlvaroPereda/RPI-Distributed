<script lang="ts">
    import type { Device } from "$lib/types";
    import ClusterCanvas from "../components/ClusterCanvas.svelte";
    import Settings from "../components/Settings.svelte";
    import SSHModel from "../components/SSHModel.svelte";

    let device: Device[] = [
        {user: "rpi5", ip: "192.168.1.10"},
        {user: "rpi3", ip: "192.168.1.11"},
        {user: "rpi3b", ip: "192.168.1.12"}
    ]

</script>
<main>
    <ClusterCanvas cluster={device}/>
    <SSHModel />
    <Settings />
</main>

<style>
    main {
        display: flex;
        position: relative;
        align-items: center;
        justify-content: center;
        width: 100vw;
        height: 100vh;
        overflow: hidden;
        padding-right: 300px; 
        box-sizing: border-box;
    }
</style>