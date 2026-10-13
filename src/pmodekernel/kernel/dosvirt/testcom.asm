[bits 16]
org 0x100

jmp a
a:
    ; mov ah, 0x09
    ; mov dl, 'a'
    ; xor ax, ax
    ; mov ah, 0x01
    ; int 0x21
    ; mov dl, al
    ; mov ah, 0x02
    ; int 0x21
    ; xor ax, ax
    ; mov ah, 0x4C
    ; int 0x21

    ; mov ah, 0xA
    ; mov dx, 0x200
    ; mov cl, 20
    ; mov [0x200], cl
    ; xor cx, cx
    ; int 0x21
    ; mov bx, dx
    ; add bx, 1
    ; mov cx, [bx]
    ; shr cx, 8
    ; add dx, cx
    ; mov byte [dx], '$'
    ; sub dx, cx
    ; mov ah, 0x09
    ; int 0x21
    ; xor ax, ax
    ; xor dx, dx
    ; mov bx, 2
    ; mov ah, 0x48
    ; int 0x21
    ; mov ds, ax
    ; mov byte [0], 'H'
    ; mov byte [1], 'E'
    ; mov byte [2], 'L'
    ; mov byte [3], 'L'
    ; mov byte [4], 'O'
    ; mov byte [5], '$'
    ; mov ah, 0x9
    ; int 0x21
    
    ; mov ah, 0x8
    ; int 0x21
    ; mov ah, 0x2
    ; mov dl, al
    ; int 0x21

    ; mov ah, 0x0A
    ; mov dx, 0x200
    ; mov cl, 20
    ; mov [0x200], cl
    ; int 0x21

    ; mov cl, '$'
    ; mov [0x20A], cl
    ; add dx, 10
    ; xor ax, ax
    ; mov ax, '$'
    ; mov [dx], ax

    ; mov ah, 0x9
    ; add dx, 2
    ; int 0x21

    ; mov ah, 0x4C
    ; int 0x21

    b:
    jmp b

teststring: db "hi :DDD $"
