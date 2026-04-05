import type { Notification, NotificationType } from "./types"

const MAX_VISIBLE = 3

class NotificationStore {
    queue = $state<Notification[]>([])
    visible = $state<Notification[]>([])

    add(type: NotificationType, message: string, timeout = 4000): void {
        const notification = { id: crypto.randomUUID(), type, message, timeout }
        if (this.visible.length < MAX_VISIBLE)
            this.visible.push(notification)
        else 
            this.queue.push(notification)
    }

    remove(id: string): void {
        this.visible = this.visible.filter(e => e.id !== id)
        if (this.queue.length > 0) {
            this.visible.push(this.queue.shift()!)
        }
    }
}

export const notificationStore = new NotificationStore()