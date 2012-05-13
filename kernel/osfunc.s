	
	extern _inthdr21, _inthdr27, _inthdr2c
	extern _inthdr20
	extern trap_handler

	global farjmp
	global load_tr, stor_tr, asm_getebp
	global asm_inthdr21, asm_inthdr27, asm_inthdr2c
	global asm_inthdr20

	; asm i/o port function for C
	global inb, outb
	global _inl, _outl

inb:
	mov  edx, [esp+4]
	mov  eax, 0
	in al, dx
	ret

outb:
	mov  edx, [esp+4]  ; io port
	mov  eax, [esp+8]  ; data
	out  dx, al
	ret

;uint32_t _intl(int port)
_inl:
	mov  edx, [esp+4]
	mov  eax, 0
	in eax, dx
	ret
;void _outl(int port, uint32_t data)
_outl:
	mov  edx, [esp+4]  ; io port
	mov  eax, [esp+8]  ; data
	out  dx, eax
	ret
; void farjmp(int eip, int cs);
; use for task switch in c
farjmp: 
		; eip = esp + 4
		; cs = esp + 8
		jmp	far	[esp+4]			; eip, cs
		ret

load_tr: ; void load_tr(int tr);
		ltr		[esp+4]			; tr
		ret

store_tr: ; void store_tr(int tr);
		str		[esp+4]			; tr
		ret

asm_getebp: ; asm_getebp(void *esp);
		mov  [esp+4], ebp
		ret

asm_inthdr20:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthdr20

	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthdr21:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax

	call	_inthdr21

	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthdr27:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthdr27
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthdr2c:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthdr2c
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

	global _all_trap_handler

;; IDT_HANDLER_NOERR(name, trapno)
%macro IDT_HANDLER_NOERR 2
	global asm_%1
asm_%1:
	push 0 ; no error code
	push %2 ; trapno
	jmp _all_trap_handler
%endmacro
%macro IDT_HANDLER 2
	global asm_%1
asm_%1:
	push %2 ; trapno
	jmp _all_trap_handler
%endmacro

IDT_HANDLER_NOERR handler_0,0
IDT_HANDLER_NOERR handler_1,1
IDT_HANDLER_NOERR handler_2,2
IDT_HANDLER_NOERR handler_3,3
IDT_HANDLER_NOERR handler_4,4
IDT_HANDLER_NOERR handler_5,5
IDT_HANDLER_NOERR handler_6,6
IDT_HANDLER_NOERR handler_7,7
IDT_HANDLER handler_8,8
IDT_HANDLER_NOERR handler_9,9
IDT_HANDLER handler_10,10
IDT_HANDLER handler_11,11
IDT_HANDLER handler_12,12
IDT_HANDLER handler_13,13
IDT_HANDLER handler_14,14
IDT_HANDLER_NOERR handler_15,15
IDT_HANDLER_NOERR handler_16,16
IDT_HANDLER handler_17,17
IDT_HANDLER_NOERR handler_18,18
IDT_HANDLER_NOERR handler_19,19
IDT_HANDLER_NOERR handler_20,20
IDT_HANDLER_NOERR handler_21,21
IDT_HANDLER_NOERR handler_22,22
IDT_HANDLER_NOERR handler_23,23
IDT_HANDLER_NOERR handler_24,24
IDT_HANDLER_NOERR handler_25,25
IDT_HANDLER_NOERR handler_26,26
IDT_HANDLER_NOERR handler_27,27
IDT_HANDLER_NOERR handler_28,28
IDT_HANDLER_NOERR handler_29,29
IDT_HANDLER_NOERR handler_30,30
IDT_HANDLER_NOERR handler_31,31
IDT_HANDLER_NOERR handler_32,32
IDT_HANDLER_NOERR handler_33,33
IDT_HANDLER_NOERR handler_34,34
IDT_HANDLER_NOERR handler_35,35
IDT_HANDLER_NOERR handler_36,36
IDT_HANDLER_NOERR handler_37,37
IDT_HANDLER_NOERR handler_38,38
IDT_HANDLER_NOERR handler_39,39
IDT_HANDLER_NOERR handler_40,40
IDT_HANDLER_NOERR handler_41,41
IDT_HANDLER_NOERR handler_42,42
IDT_HANDLER_NOERR handler_43,43
IDT_HANDLER_NOERR handler_44,44
IDT_HANDLER_NOERR handler_45,45
IDT_HANDLER_NOERR handler_46,46
IDT_HANDLER_NOERR handler_47,47
IDT_HANDLER_NOERR handler_48,48
IDT_HANDLER_NOERR handler_49,49
IDT_HANDLER_NOERR handler_50,50
IDT_HANDLER_NOERR handler_51,51
IDT_HANDLER_NOERR handler_52,52
IDT_HANDLER_NOERR handler_53,53
IDT_HANDLER_NOERR handler_54,54
IDT_HANDLER_NOERR handler_55,55
IDT_HANDLER_NOERR handler_56,56
IDT_HANDLER_NOERR handler_57,57
IDT_HANDLER_NOERR handler_58,58
IDT_HANDLER_NOERR handler_59,59
IDT_HANDLER_NOERR handler_60,60
IDT_HANDLER_NOERR handler_61,61
IDT_HANDLER_NOERR handler_62,62
IDT_HANDLER_NOERR handler_63,63
IDT_HANDLER_NOERR handler_64,64
IDT_HANDLER_NOERR handler_65,65
IDT_HANDLER_NOERR handler_66,66
IDT_HANDLER_NOERR handler_67,67
IDT_HANDLER_NOERR handler_68,68
IDT_HANDLER_NOERR handler_69,69
IDT_HANDLER_NOERR handler_70,70
IDT_HANDLER_NOERR handler_71,71
IDT_HANDLER_NOERR handler_72,72
IDT_HANDLER_NOERR handler_73,73
IDT_HANDLER_NOERR handler_74,74
IDT_HANDLER_NOERR handler_75,75
IDT_HANDLER_NOERR handler_76,76
IDT_HANDLER_NOERR handler_77,77
IDT_HANDLER_NOERR handler_78,78
IDT_HANDLER_NOERR handler_79,79
IDT_HANDLER_NOERR handler_80,80
IDT_HANDLER_NOERR handler_81,81
IDT_HANDLER_NOERR handler_82,82
IDT_HANDLER_NOERR handler_83,83
IDT_HANDLER_NOERR handler_84,84
IDT_HANDLER_NOERR handler_85,85
IDT_HANDLER_NOERR handler_86,86
IDT_HANDLER_NOERR handler_87,87
IDT_HANDLER_NOERR handler_88,88
IDT_HANDLER_NOERR handler_89,89
IDT_HANDLER_NOERR handler_90,90
IDT_HANDLER_NOERR handler_91,91
IDT_HANDLER_NOERR handler_92,92
IDT_HANDLER_NOERR handler_93,93
IDT_HANDLER_NOERR handler_94,94
IDT_HANDLER_NOERR handler_95,95
IDT_HANDLER_NOERR handler_96,96
IDT_HANDLER_NOERR handler_97,97
IDT_HANDLER_NOERR handler_98,98
IDT_HANDLER_NOERR handler_99,99
IDT_HANDLER_NOERR handler_100,100
IDT_HANDLER_NOERR handler_101,101
IDT_HANDLER_NOERR handler_102,102
IDT_HANDLER_NOERR handler_103,103
IDT_HANDLER_NOERR handler_104,104
IDT_HANDLER_NOERR handler_105,105
IDT_HANDLER_NOERR handler_106,106
IDT_HANDLER_NOERR handler_107,107
IDT_HANDLER_NOERR handler_108,108
IDT_HANDLER_NOERR handler_109,109
IDT_HANDLER_NOERR handler_110,110
IDT_HANDLER_NOERR handler_111,111
IDT_HANDLER_NOERR handler_112,112
IDT_HANDLER_NOERR handler_113,113
IDT_HANDLER_NOERR handler_114,114
IDT_HANDLER_NOERR handler_115,115
IDT_HANDLER_NOERR handler_116,116
IDT_HANDLER_NOERR handler_117,117
IDT_HANDLER_NOERR handler_118,118
IDT_HANDLER_NOERR handler_119,119
IDT_HANDLER_NOERR handler_120,120
IDT_HANDLER_NOERR handler_121,121
IDT_HANDLER_NOERR handler_122,122
IDT_HANDLER_NOERR handler_123,123
IDT_HANDLER_NOERR handler_124,124
IDT_HANDLER_NOERR handler_125,125
IDT_HANDLER_NOERR handler_126,126
IDT_HANDLER_NOERR handler_127,127
IDT_HANDLER_NOERR handler_128,128
IDT_HANDLER_NOERR handler_129,129
IDT_HANDLER_NOERR handler_130,130
IDT_HANDLER_NOERR handler_131,131
IDT_HANDLER_NOERR handler_132,132
IDT_HANDLER_NOERR handler_133,133
IDT_HANDLER_NOERR handler_134,134
IDT_HANDLER_NOERR handler_135,135
IDT_HANDLER_NOERR handler_136,136
IDT_HANDLER_NOERR handler_137,137
IDT_HANDLER_NOERR handler_138,138
IDT_HANDLER_NOERR handler_139,139
IDT_HANDLER_NOERR handler_140,140
IDT_HANDLER_NOERR handler_141,141
IDT_HANDLER_NOERR handler_142,142
IDT_HANDLER_NOERR handler_143,143
IDT_HANDLER_NOERR handler_144,144
IDT_HANDLER_NOERR handler_145,145
IDT_HANDLER_NOERR handler_146,146
IDT_HANDLER_NOERR handler_147,147
IDT_HANDLER_NOERR handler_148,148
IDT_HANDLER_NOERR handler_149,149
IDT_HANDLER_NOERR handler_150,150
IDT_HANDLER_NOERR handler_151,151
IDT_HANDLER_NOERR handler_152,152
IDT_HANDLER_NOERR handler_153,153
IDT_HANDLER_NOERR handler_154,154
IDT_HANDLER_NOERR handler_155,155
IDT_HANDLER_NOERR handler_156,156
IDT_HANDLER_NOERR handler_157,157
IDT_HANDLER_NOERR handler_158,158
IDT_HANDLER_NOERR handler_159,159
IDT_HANDLER_NOERR handler_160,160
IDT_HANDLER_NOERR handler_161,161
IDT_HANDLER_NOERR handler_162,162
IDT_HANDLER_NOERR handler_163,163
IDT_HANDLER_NOERR handler_164,164
IDT_HANDLER_NOERR handler_165,165
IDT_HANDLER_NOERR handler_166,166
IDT_HANDLER_NOERR handler_167,167
IDT_HANDLER_NOERR handler_168,168
IDT_HANDLER_NOERR handler_169,169
IDT_HANDLER_NOERR handler_170,170
IDT_HANDLER_NOERR handler_171,171
IDT_HANDLER_NOERR handler_172,172
IDT_HANDLER_NOERR handler_173,173
IDT_HANDLER_NOERR handler_174,174
IDT_HANDLER_NOERR handler_175,175
IDT_HANDLER_NOERR handler_176,176
IDT_HANDLER_NOERR handler_177,177
IDT_HANDLER_NOERR handler_178,178
IDT_HANDLER_NOERR handler_179,179
IDT_HANDLER_NOERR handler_180,180
IDT_HANDLER_NOERR handler_181,181
IDT_HANDLER_NOERR handler_182,182
IDT_HANDLER_NOERR handler_183,183
IDT_HANDLER_NOERR handler_184,184
IDT_HANDLER_NOERR handler_185,185
IDT_HANDLER_NOERR handler_186,186
IDT_HANDLER_NOERR handler_187,187
IDT_HANDLER_NOERR handler_188,188
IDT_HANDLER_NOERR handler_189,189
IDT_HANDLER_NOERR handler_190,190
IDT_HANDLER_NOERR handler_191,191
IDT_HANDLER_NOERR handler_192,192
IDT_HANDLER_NOERR handler_193,193
IDT_HANDLER_NOERR handler_194,194
IDT_HANDLER_NOERR handler_195,195
IDT_HANDLER_NOERR handler_196,196
IDT_HANDLER_NOERR handler_197,197
IDT_HANDLER_NOERR handler_198,198
IDT_HANDLER_NOERR handler_199,199
IDT_HANDLER_NOERR handler_200,200
IDT_HANDLER_NOERR handler_201,201
IDT_HANDLER_NOERR handler_202,202
IDT_HANDLER_NOERR handler_203,203
IDT_HANDLER_NOERR handler_204,204
IDT_HANDLER_NOERR handler_205,205
IDT_HANDLER_NOERR handler_206,206
IDT_HANDLER_NOERR handler_207,207
IDT_HANDLER_NOERR handler_208,208
IDT_HANDLER_NOERR handler_209,209
IDT_HANDLER_NOERR handler_210,210
IDT_HANDLER_NOERR handler_211,211
IDT_HANDLER_NOERR handler_212,212
IDT_HANDLER_NOERR handler_213,213
IDT_HANDLER_NOERR handler_214,214
IDT_HANDLER_NOERR handler_215,215
IDT_HANDLER_NOERR handler_216,216
IDT_HANDLER_NOERR handler_217,217
IDT_HANDLER_NOERR handler_218,218
IDT_HANDLER_NOERR handler_219,219
IDT_HANDLER_NOERR handler_220,220
IDT_HANDLER_NOERR handler_221,221
IDT_HANDLER_NOERR handler_222,222
IDT_HANDLER_NOERR handler_223,223
IDT_HANDLER_NOERR handler_224,224
IDT_HANDLER_NOERR handler_225,225
IDT_HANDLER_NOERR handler_226,226
IDT_HANDLER_NOERR handler_227,227
IDT_HANDLER_NOERR handler_228,228
IDT_HANDLER_NOERR handler_229,229
IDT_HANDLER_NOERR handler_230,230
IDT_HANDLER_NOERR handler_231,231
IDT_HANDLER_NOERR handler_232,232
IDT_HANDLER_NOERR handler_233,233
IDT_HANDLER_NOERR handler_234,234
IDT_HANDLER_NOERR handler_235,235
IDT_HANDLER_NOERR handler_236,236
IDT_HANDLER_NOERR handler_237,237
IDT_HANDLER_NOERR handler_238,238
IDT_HANDLER_NOERR handler_239,239
IDT_HANDLER_NOERR handler_240,240
IDT_HANDLER_NOERR handler_241,241
IDT_HANDLER_NOERR handler_242,242
IDT_HANDLER_NOERR handler_243,243
IDT_HANDLER_NOERR handler_244,244
IDT_HANDLER_NOERR handler_245,245
IDT_HANDLER_NOERR handler_246,246
IDT_HANDLER_NOERR handler_247,247
IDT_HANDLER_NOERR handler_248,248
IDT_HANDLER_NOERR handler_249,249
IDT_HANDLER_NOERR handler_250,250
IDT_HANDLER_NOERR handler_251,251
IDT_HANDLER_NOERR handler_252,252
IDT_HANDLER_NOERR handler_253,253
IDT_HANDLER_NOERR handler_254,254
IDT_HANDLER_NOERR handler_255,255

_all_trap_handler:
	push eax
	push edx
	push ecx
	
	push esp
	call trap_handler
	add  esp, 4

	pop ecx
	pop edx 
	add esp, 4 ; no pop eax for return code
	add esp, 8
	iretd

	global trap_handlers
trap_handlers:
	dd asm_handler_0
	dd asm_handler_1
	dd asm_handler_2
	dd asm_handler_3
	dd asm_handler_4
	dd asm_handler_5
	dd asm_handler_6
	dd asm_handler_7
	dd asm_handler_8
	dd asm_handler_9
	dd asm_handler_10
	dd asm_handler_11
	dd asm_handler_12
	dd asm_handler_13
	dd asm_handler_14
	dd asm_handler_15
	dd asm_handler_16
	dd asm_handler_17
	dd asm_handler_18
	dd asm_handler_19
	dd asm_handler_20
	dd asm_handler_21
	dd asm_handler_22
	dd asm_handler_23
	dd asm_handler_24
	dd asm_handler_25
	dd asm_handler_26
	dd asm_handler_27
	dd asm_handler_28
	dd asm_handler_29
	dd asm_handler_30
	dd asm_handler_31
	dd asm_handler_32
	dd asm_handler_33
	dd asm_handler_34
	dd asm_handler_35
	dd asm_handler_36
	dd asm_handler_37
	dd asm_handler_38
	dd asm_handler_39
	dd asm_handler_40
	dd asm_handler_41
	dd asm_handler_42
	dd asm_handler_43
	dd asm_handler_44
	dd asm_handler_45
	dd asm_handler_46
	dd asm_handler_47
	dd asm_handler_48
	dd asm_handler_49
	dd asm_handler_50
	dd asm_handler_51
	dd asm_handler_52
	dd asm_handler_53
	dd asm_handler_54
	dd asm_handler_55
	dd asm_handler_56
	dd asm_handler_57
	dd asm_handler_58
	dd asm_handler_59
	dd asm_handler_60
	dd asm_handler_61
	dd asm_handler_62
	dd asm_handler_63
	dd asm_handler_64
	dd asm_handler_65
	dd asm_handler_66
	dd asm_handler_67
	dd asm_handler_68
	dd asm_handler_69
	dd asm_handler_70
	dd asm_handler_71
	dd asm_handler_72
	dd asm_handler_73
	dd asm_handler_74
	dd asm_handler_75
	dd asm_handler_76
	dd asm_handler_77
	dd asm_handler_78
	dd asm_handler_79
	dd asm_handler_80
	dd asm_handler_81
	dd asm_handler_82
	dd asm_handler_83
	dd asm_handler_84
	dd asm_handler_85
	dd asm_handler_86
	dd asm_handler_87
	dd asm_handler_88
	dd asm_handler_89
	dd asm_handler_90
	dd asm_handler_91
	dd asm_handler_92
	dd asm_handler_93
	dd asm_handler_94
	dd asm_handler_95
	dd asm_handler_96
	dd asm_handler_97
	dd asm_handler_98
	dd asm_handler_99
	dd asm_handler_100
	dd asm_handler_101
	dd asm_handler_102
	dd asm_handler_103
	dd asm_handler_104
	dd asm_handler_105
	dd asm_handler_106
	dd asm_handler_107
	dd asm_handler_108
	dd asm_handler_109
	dd asm_handler_110
	dd asm_handler_111
	dd asm_handler_112
	dd asm_handler_113
	dd asm_handler_114
	dd asm_handler_115
	dd asm_handler_116
	dd asm_handler_117
	dd asm_handler_118
	dd asm_handler_119
	dd asm_handler_120
	dd asm_handler_121
	dd asm_handler_122
	dd asm_handler_123
	dd asm_handler_124
	dd asm_handler_125
	dd asm_handler_126
	dd asm_handler_127
	dd asm_handler_128
	dd asm_handler_129
	dd asm_handler_130
	dd asm_handler_131
	dd asm_handler_132
	dd asm_handler_133
	dd asm_handler_134
	dd asm_handler_135
	dd asm_handler_136
	dd asm_handler_137
	dd asm_handler_138
	dd asm_handler_139
	dd asm_handler_140
	dd asm_handler_141
	dd asm_handler_142
	dd asm_handler_143
	dd asm_handler_144
	dd asm_handler_145
	dd asm_handler_146
	dd asm_handler_147
	dd asm_handler_148
	dd asm_handler_149
	dd asm_handler_150
	dd asm_handler_151
	dd asm_handler_152
	dd asm_handler_153
	dd asm_handler_154
	dd asm_handler_155
	dd asm_handler_156
	dd asm_handler_157
	dd asm_handler_158
	dd asm_handler_159
	dd asm_handler_160
	dd asm_handler_161
	dd asm_handler_162
	dd asm_handler_163
	dd asm_handler_164
	dd asm_handler_165
	dd asm_handler_166
	dd asm_handler_167
	dd asm_handler_168
	dd asm_handler_169
	dd asm_handler_170
	dd asm_handler_171
	dd asm_handler_172
	dd asm_handler_173
	dd asm_handler_174
	dd asm_handler_175
	dd asm_handler_176
	dd asm_handler_177
	dd asm_handler_178
	dd asm_handler_179
	dd asm_handler_180
	dd asm_handler_181
	dd asm_handler_182
	dd asm_handler_183
	dd asm_handler_184
	dd asm_handler_185
	dd asm_handler_186
	dd asm_handler_187
	dd asm_handler_188
	dd asm_handler_189
	dd asm_handler_190
	dd asm_handler_191
	dd asm_handler_192
	dd asm_handler_193
	dd asm_handler_194
	dd asm_handler_195
	dd asm_handler_196
	dd asm_handler_197
	dd asm_handler_198
	dd asm_handler_199
	dd asm_handler_200
	dd asm_handler_201
	dd asm_handler_202
	dd asm_handler_203
	dd asm_handler_204
	dd asm_handler_205
	dd asm_handler_206
	dd asm_handler_207
	dd asm_handler_208
	dd asm_handler_209
	dd asm_handler_210
	dd asm_handler_211
	dd asm_handler_212
	dd asm_handler_213
	dd asm_handler_214
	dd asm_handler_215
	dd asm_handler_216
	dd asm_handler_217
	dd asm_handler_218
	dd asm_handler_219
	dd asm_handler_220
	dd asm_handler_221
	dd asm_handler_222
	dd asm_handler_223
	dd asm_handler_224
	dd asm_handler_225
	dd asm_handler_226
	dd asm_handler_227
	dd asm_handler_228
	dd asm_handler_229
	dd asm_handler_230
	dd asm_handler_231
	dd asm_handler_232
	dd asm_handler_233
	dd asm_handler_234
	dd asm_handler_235
	dd asm_handler_236
	dd asm_handler_237
	dd asm_handler_238
	dd asm_handler_239
	dd asm_handler_240
	dd asm_handler_241
	dd asm_handler_242
	dd asm_handler_243
	dd asm_handler_244
	dd asm_handler_245
	dd asm_handler_246
	dd asm_handler_247
	dd asm_handler_248
	dd asm_handler_249
	dd asm_handler_250
	dd asm_handler_251
	dd asm_handler_252
	dd asm_handler_253
	dd asm_handler_254
	dd asm_handler_255
