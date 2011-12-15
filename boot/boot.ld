SECTIONS {
	. = 0x7c00;

	.text : {
		.begin = .;
		bpb.o  (.text);
		boot.o (.text);
		*      (.text);
		. = .begin + 510;
		bpb.o  (.signature);
	}
}
