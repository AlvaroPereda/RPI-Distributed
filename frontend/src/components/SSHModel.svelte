<script lang="ts">
    import type { Device } from "$lib/types";

    let device: Device = $state({ user: "", ip: "", password: "" });

    const connectDevice = async() => {
        const response = await fetch("/connect", {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                user: device.user,
                ip: device.ip,
                password: device.password
            })
        })

        if(!response.ok) throw new Error("Error en la respuesta del servidor.")
    }
</script>

<div>
    <form>
        <label for="name">Name:</label>
        <input type="text" id="name" bind:value={device.user}>
        <label for="ip">IP:</label>
        <input type="text" id="ip" placeholder="192.168.x.x" bind:value={device.ip}>
        <label for="password">Password:</label>
        <input type="password" id="password" bind:value={device.password}>
        <button type="button" onclick={connectDevice}>Connect</button>
    </form>
</div>

<style>
    div {
        position: absolute;
        z-index: 1;
        form {
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
        }
    }
</style>