export declare type Board = {
    links: number;
    components: {
        type: ComponentType;
        inputs: number[];
        outputs: number[];
    }[];
}

export declare type BoardStatus = {
    currentSpeed: number;
    currentState: number;
    threadCount: number;
    componentCount: number;
    linkCount: number;
    tick: number;
}

export declare type BoardState = {
    components: boolean[][];
    links: boolean[];
}

export declare const enum ComponentType {
    NOT = 1,
    AND = 2,
    OR = 3,
    XOR = 4,
    DELAY = 5,
    CLK = 6,
    HalfAdder = 10,
    FullAdder = 11,
    ROM = 12,
    D_FF = 13,
    JK_FF = 14,
    SR_FF = 15,
    UserInput = 200
}

export declare const enum InputEvent {
    Cont,
    Pulse
}

export declare module logicsim {
    function init(board: Board): void;
    function destroy(): void;
    function start(threads?: number, ticks?: number, ms?: number, synchronized?: boolean): void;
    function stop(): void;
    function getStatus(): BoardStatus;
    function getBoard(): BoardState;
    function getLinks(): boolean[];
    function triggerInput(componentIndex: number, inputEvent: InputEvent, state: boolean[]): void;
}
