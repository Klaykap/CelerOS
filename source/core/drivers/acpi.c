//CELEROS

uint32_t rsdp_offset=0;
uint32_t rsdt_offset=0;
uint32_t facp_offset=0;

struct acpi_tables_t {
	uint8_t name[4];
	uint32_t pointer;
} acpi_tables[25];

void acpi(void) {
	//search for RSDP singature "RSD PTR"
	for(int i=0; i<0x20000; i++) {
		if( mmio_inl(0xE0000, i)==0x20445352 && mmio_inl(0xE0000, i+4)==0x20525450) {
			p("ACPI: Found at BIOS MAIN AREA");
			rsdp_offset=(0xE0000+i);
			goto read_acpi;
		}
	}
	for(int i=0; i<0x20000; i++) {
		if( mmio_inl(0x80000, i)==0x20445352 && mmio_inl(0xE0000, i+4)==0x20525450) {
			p("ACPI: Found at EBDA");
			rsdp_offset=(0x80000+i);
			goto read_acpi;
		}
	}
	p("ACPI: not found");
	return;
	
	read_acpi:
	p("ACPI: RSDP offset");
	ph(rsdp_offset);
	p("ACPI: RSDT offset");
	if(mmio_inb(rsdp_offset, 15)==0) {
p("ACPI 1");
		rsdt_offset = mmio_inl(rsdp_offset, 16);
	}
	else {
p("ACPI 2");
		rsdt_offset = mmio_inl(rsdp_offset, 24);
	}
	ph(rsdt_offset);

	//get entries
	for(int i=0; i<25; i++) {
		facp_offset=mmio_inl(rsdt_offset, 40+i*4);
		if(facp_offset!=0) {
			acpi_tables[i].name[0]=mmio_inb(facp_offset, 0);
			acpi_tables[i].name[1]=mmio_inb(facp_offset, 1);
			acpi_tables[i].name[2]=mmio_inb(facp_offset, 2);
			acpi_tables[i].name[3]=mmio_inb(facp_offset, 3);
			acpi_tables[i].name[4]=0;

			acpi_tables[i].pointer=facp_offset;
	//p((uint32_t)&acpi_tables[i].name);
	ph(acpi_tables[i].pointer);
		}
	}
	p("ACPI: FACP not found");
	return;

	read_facp:
	p("ACPI: reading FACP");
}
