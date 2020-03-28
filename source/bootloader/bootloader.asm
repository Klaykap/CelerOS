org 0x7C00

bits 16

start:
  jmp main
  TIMES 3-($-$$) DB 0x90

  ;Fake BPB for BIOS
  db "mkfs.fat"
  dw 512
  db 1
  dw 1
  db 2
  dw 224
  dw 2880
  db 0xF0
  dw 9
  dw 18
  dw 2
  dd 0
  dd 0
  db 0
  db 0
  db 0x29
  dd 0x2D7E5A1A
  db "CELEROS    "
  db "FAT12   "

main:
  xor ax, ax
  mov ds, ax
  mov ss, ax
  mov sp, 0x7C00

  ;VESA mode info
  mov ax, 0
  mov es, ax
  mov di, 0x3000  ;offset 3000
  mov ax, 0x4F00 ;get VESA info block
  int 0x10 ;load info

  ;load VESA info block
  mov ax, 0  ;segment 0
  mov es, ax
  mov di, 0x1000  ;offset 1000
  mov ax, 0x4F01  ;get VBE mode information block
  mov cx, 0x4114  ;graphic mode 800x600 16 bpp
  int 0x10  ;load informations

  ;set VESA video mode
  mov ax, 0x4F02  ;set VBE graphic mode
  mov bx, 0x4114  ;graphic mode 800x600 16 bpp
  int 0x10  ;start BIOS interrupt

  ;VBE3 portected mode table
  mov ax, 0x4F0A
  mov bl, 0
  int 0x10

  mov bx, 0x2000
  mov word [bx], ax
  add bx, 2
  mov word [bx], es
  add bx, 2
  mov word [bx], di
  add bx, 2
  mov word [bx], cx

  ;loading 128 sectors from usb/cdrom

  ;read usb
  mov ax, 0x1000  ;load kernel to 10000h
  mov es, ax  ;load kernel to 10000h
  mov bx, 0  ;offset 0 
  mov ah, 0x02  ;read function
  mov al, 72  ;two cylinders
  mov ch, 0  ;cylinder 0
  mov dh, 0  ;head 0
  mov cl, 2  ;start sector is 2
  mov dl, 0x00  ;usb
  int 13h  ;read

  ;read usb
  mov ax, 0x1000  ;load kernel to 10000h
  mov es, ax  ;load kernel to 10000h
  mov bx, 36864  ;offset 36864 
  mov ah, 0x02  ;read function
  mov al, 56  ;to end of segment
  mov ch, 2  ;cylinder 0
  mov dh, 0  ;head 0
  mov cl, 2  ;start sector is 2
  mov dl, 0x00  ;usb
  int 13h  ;read

  jmp load_gdt 

  ;global descriptor table
  gdt:

  gdt_null:
  dq 0

  gdt_code:
  dw 0xFFFF
  dw 0

  db 0
  db 0x9A
  db 0xCF
  db 0

  gdt_data:
  dw 0xFFFF
  dw 0

  db 0
  db 0x92
  db 0xCF
  db 0

  gdt_end:

  gdt_desc:
   dw gdt_end - gdt - 1
   dd gdt

  ;load gdt
  load_gdt:
    cli
    lgdt [gdt_desc]

  ;jump into protected mode
  mov eax, cr0
  or eax, 1
  mov cr0, eax
  jmp 0x0008:clear_pipe

  ;NOW WE ARE IN PROTECTED MODE
  bits 32

  clear_pipe:
  mov ax, 0x0010
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov esp, 0x80000  ;set stack pointer

  ;copy system to memory 0x200000
  mov ecx, 64000
  mov dword [copy_offset], 0
  copy:
    mov ebx, 0x10000
    add ebx, dword [copy_offset]
    mov eax, dword [ebx]

    mov ebx, 0x200000
    add ebx, dword [copy_offset]
    mov dword [ebx], eax

    add dword [copy_offset], 4
  loop copy

  ;JUMP TO EXECUTE CELEROS!
  jmp 0x200000

  copy_offset dd 0

times 510-($-$$) db 0
dw 0xAA55
