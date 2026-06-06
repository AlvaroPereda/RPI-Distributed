export type Metrics = {
    predicted_per_second: number,
    predicted_ms: number,
    predicted_n: number
}

type PromptRole = "user" | "assistant" | "system"

export type Prompt = {
    role: PromptRole,
    content: string,
    metrics?: Metrics
}

export type LlamaAPI = {
    choices: {
        delta: {
            content: string
        },
        finish_reason: string
    }[],
    model: string,
    timings: Metrics
}

export type Device = {
    user: string,
    ip: string,
    password?: string
}

export type NotificationType = "success" | "error" | "info"

export type Notification = {
    id: string,
    type: NotificationType,
    message: string,
    timeout: number,
}