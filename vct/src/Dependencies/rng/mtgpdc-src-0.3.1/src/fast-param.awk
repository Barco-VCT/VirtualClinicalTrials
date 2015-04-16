#!/usr/bin/awk -f
# sample awk file to make C table source
BEGIN {
    FS=","
    first = 1;
    count = 0;
}
END {
    printf("\n};\n");
    printf("const int mtgp%sdc_params_%s_num  = %d;\n", w, mexp, count);
}

/^#/ {next}
{
    if (first) {
	first = 0;
	first_out($2, $3);
    }
    middle_out();
}

function first_out(mexp, type,	w, n, i, p) {
    w = substr(type, 5, 2);
    n =  int(mexp / w) + 1;
    p = 2;
    for (p = 1; p < n; p = p * 2);
    printf("#include <stdint.h>\n");
    printf("#include \"mtgp%s-fast.h\"\n", w);
    printf("#define MTGPDC_MEXP %s\n", mexp);
    printf("#define MTGPDC_N %s\n", n);
    printf("#define MTGPDC_FLOOR_2P %s\n", p / 2);
    printf("#define MTGPDC_CEIL_2P %s\n", p);
    printf("#define MTGPDC_PARAM_TABLE mtgp%sdc_params_fast_%s\n", w, mexp);
    printf("\n");
    printf("mtgp%s_params_fast_t mtgp%sdc_params_fast_%s[]\n = {", w, w, mexp);
    connect = "\n";
}

function middle_out() {
    printf("%s", connect);
    sha1 = $1;
    sha1 = substr(sha1, 2);
    mexp = $2;
    type = $3;
    id = $4;
    pos = $5;
    sh1 = $6;
    sh2 = $7;
    mask = $16;
    weight = $17;
    delta = $18;
    printf("    {\n");
    printf("        /* No.%s delta:%s weight:%s */\n", id, delta, weight);
    printf("        %s,\n", mexp);
    printf("        %s,\n", pos);
    printf("        %s,\n", sh1);
    printf("        %s,\n", sh2);
    printf("        {");
    if (type == "uint32_t") {
	fmt = "UINT32_C(%s),\n";
	fmt2 = "UINT32_C(%s)";
    } else {
	fmt = "UINT64_C(%s),\n";
	fmt2 = "UINT64_C(%s)";
    }
    x = 19;
    for (i = 0; i < 15; i++) {
	printf(fmt, $(x++));
	printf("         ");
    }
    printf(fmt2, $(x++));
    printf("},\n");
    printf("        {");
    for (i = 0; i < 15; i++) {
	printf(fmt, $(x++));
	printf("         ");
    }
    printf(fmt2, $(x++));
    printf("},\n");
    printf("        {");
    for (i = 0; i < 15; i++) {
	printf(fmt, $(x++));
	printf("         ");
    }
    printf(fmt2, $(x++));
    printf("},\n");
    printf("        ");
    printf(fmt, mask);
    printf("        {");
    for (i = 0; i < 10; i++) {
	printf("0x%s,", substr(sha1, 1, 2));
	sha1 = substr(sha1, 3);
    }
    printf("\n");
    printf("         ");
    for (i = 0; i < 10; i++) {
	printf("0x%s,", substr(sha1, 1, 2));
	sha1 = substr(sha1, 3);
    }
    printf("0x00}\n");
    printf("    }");
    connect = ",\n";
    count++;
}

