import type { Device } from "./types";

class DeviceState {
    current = $state<Device[]>([])

    setConnection(data: Device) {
        this.current.push(data)
    }
}

export const deviceState = new DeviceState()