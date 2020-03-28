//CELEROS

#define FAT_HD 1

struct fat_info_t {
	uint8_t jmp[5];
	uint8_t oem[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t number_of_fat_tables;
	uint16_t root_entries;
	uint16_t sectors_low;
	uint8_t media_descriptor_type;
	uint16_t sectors_per_old_fat;
	uint16_t number_of_sectors_per_track;
	uint16_t number_of_heads;
	uint32_t number_of_hidden_sectors;
	uint32_t large_sector_count;
	uint8_t code[512];

	uint32_t sectors_per_fat;
	uint32_t root_folder_cluster;
} fat_info;

struct fat_entry_t {
	uint8_t name[8];
	uint8_t extension[3];
	uint8_t attributes;
	uint16_t created_time;
	uint16_t created_date;
	uint32_t cluster;
	uint32_t size;

	uint8_t lfs_name[13];
} fat_entry[1000];

uint8_t fat_info_data[512];
uint32_t fat_table_data_sector[128];
uint32_t fat_file_clusters[100000];
uint32_t fat_file_free_clusters[100000];

uint32_t fat_device=0;
uint32_t fat_table_start=0;
uint32_t fat_data_start=0;
uint32_t load_fat_table_sector=0;

uint8_t fat_file[4000];

void fat_select_device(uint32_t device) {
	fat_device=device;
}

uint32_t fat_read(uint64_t sector, uint16_t number_of_sectors, uint32_t memory) {
	uint32_t value=0;

	if(fat_device==FAT_HD) {
		value = hard_disc(IDE_READ, sector, number_of_sectors, memory);
	}

	return value;
}

uint32_t fat_write(uint64_t sector, uint16_t number_of_sectors, uint32_t memory) {
	uint32_t value=0;

	if(fat_device==FAT_HD) {
		value = hard_disc(IDE_WRITE, sector, number_of_sectors, memory);
	}

	return value;
}

void fat_read_info(void) {
	fat_read(0, 1, (uint32_t)&fat_info_data);

	//base FAT info
	for(int i=0; i<8; i++) {
		fat_info.oem[i]=mmio_inb((uint32_t)&fat_info_data, i+3);
	}
	fat_info.bytes_per_sector=mmio_inw((uint32_t)&fat_info_data, 11);
	fat_info.sectors_per_cluster=mmio_inb((uint32_t)&fat_info_data, 13);
	fat_info.reserved_sectors=mmio_inw((uint32_t)&fat_info_data, 14);
	fat_info.number_of_fat_tables=mmio_inb((uint32_t)&fat_info_data, 16);
	fat_info.root_entries=mmio_inw((uint32_t)&fat_info_data, 17);
	fat_info.sectors_low=mmio_inw((uint32_t)&fat_info_data, 19);
	fat_info.media_descriptor_type=mmio_inb((uint32_t)&fat_info_data, 21);
	fat_info.sectors_per_old_fat=mmio_inw((uint32_t)&fat_info_data, 22);
	fat_info.number_of_sectors_per_track=mmio_inw((uint32_t)&fat_info_data, 24);
	fat_info.number_of_heads=mmio_inw((uint32_t)&fat_info_data, 26);
	fat_info.number_of_hidden_sectors=mmio_inl((uint32_t)&fat_info_data, 28);
	fat_info.large_sector_count=mmio_inl((uint32_t)&fat_info_data, 32);

	//FAT32 info
	fat_info.sectors_per_fat=mmio_inl((uint32_t)&fat_info_data, 36);
	fat_info.root_folder_cluster=mmio_inl((uint32_t)&fat_info_data, 44);

	//init base values
	fat_table_start=fat_info.reserved_sectors;
	fat_data_start=(fat_table_start + (fat_info.number_of_fat_tables * fat_info.sectors_per_old_fat));
}

uint32_t get_fat_cluster(uint32_t cluster) {
	uint32_t cluster_sector=(cluster/128);

	if(load_fat_table_sector==cluster_sector) {
		return fat_table_data_sector[(cluster - (cluster/128*128))];
	}
	else {
		fat_read( (fat_table_start + cluster_sector), 1, (uint32_t)&fat_table_data_sector);
		load_fat_table_sector=cluster_sector;
		return fat_table_data_sector[(cluster - (cluster/128*128))];
	}
}

void set_fat_cluster(uint32_t cluster, uint32_t value) {
	uint32_t cluster_sector=(cluster/128);

	if(load_fat_table_sector==cluster_sector) {
		fat_table_data_sector[(cluster - (cluster/128*128))]=value;
		fat_write( (fat_table_start + cluster_sector), 1, (uint32_t)&fat_table_data_sector);
	}
	else {
		fat_read( (fat_table_start + cluster_sector), 1, (uint32_t)&fat_table_data_sector);
		load_fat_table_sector=cluster_sector;
		fat_table_data_sector[(cluster - (cluster/128*128))]=value;
		fat_write( (fat_table_start + cluster_sector), 1, (uint32_t)&fat_table_data_sector);
	}
}

uint32_t read_fat_cluster(uint32_t cluster, uint32_t memory) {
	uint32_t value=0;
	value = fat_read( (fat_data_start + (cluster - 2)*fat_info.sectors_per_cluster), fat_info.sectors_per_cluster, memory);
	return value;
}

uint32_t write_fat_cluster(uint32_t cluster, uint32_t memory) {
	uint32_t value=0;
	value = fat_read( (fat_data_start + (cluster - 2)*fat_info.sectors_per_cluster), fat_info.sectors_per_cluster, memory);
	return value;
}

uint32_t fat_read_file(uint32_t start_cluster, uint32_t memory) {
	uint32_t actual_cluster=0;
	uint32_t read=0;

	for(int i=0; i<100000; i++) {
		actual_cluster=get_fat_cluster(start_cluster);
		fat_file_clusters[i]=start_cluster;
		if(actual_cluster>0x0FFFFFF7) { //end of file
			goto read_fat_file_data;
		}
		start_cluster=actual_cluster;
	}
	read_fat_file_data:

	for(int i=0; i<100000; i++) {
		if(fat_file_clusters[i]==0) {
			return 1;
		}
		read = read_fat_cluster(fat_file_clusters[i], memory+(i*fat_info.sectors_per_cluster));
		if(read==0) {
			p("ERROR by reading in FAT32");
		}
	}
}

void read_fat_entry(uint32_t entry, uint32_t memory) {
	for(int i=0; i<8; i++) {
		fat_entry[entry].name[i]=mmio_inb(memory+(entry*32), i);
	}
	fat_entry[entry].name[8]=0;

	for(int i=0; i<3; i++) {
		fat_entry[entry].extension[i]=mmio_inb(memory+(entry*32), i+8);
	}
	fat_entry[entry].extension[3]=0;

	fat_entry[entry].attributes=mmio_inb(memory+(entry*32), 11);
	fat_entry[entry].created_time=mmio_inw(memory+(entry*32), 14);
	fat_entry[entry].created_date=mmio_inw(memory+(entry*32), 16);
	fat_entry[entry].cluster=( (mmio_inw(memory+(entry*32), 20) << 16) | mmio_inw(memory+(entry*32), 26));
	fat_entry[entry].size=mmio_inl(memory+(entry*32), 28);

	if(fat_entry[entry].attributes==0x0F) { //long file name
		fat_entry[entry].lfs_name[0]=mmio_inb(memory+(entry*32), 1);
		fat_entry[entry].lfs_name[1]=mmio_inb(memory+(entry*32), 3);
		fat_entry[entry].lfs_name[2]=mmio_inb(memory+(entry*32), 5);
		fat_entry[entry].lfs_name[3]=mmio_inb(memory+(entry*32), 7);
		fat_entry[entry].lfs_name[4]=mmio_inb(memory+(entry*32), 9);

		fat_entry[entry].lfs_name[5]=mmio_inb(memory+(entry*32), 14);
		fat_entry[entry].lfs_name[6]=mmio_inb(memory+(entry*32), 16);
		fat_entry[entry].lfs_name[7]=mmio_inb(memory+(entry*32), 18);
		fat_entry[entry].lfs_name[8]=mmio_inb(memory+(entry*32), 20);
		fat_entry[entry].lfs_name[9]=mmio_inb(memory+(entry*32), 22);
		fat_entry[entry].lfs_name[10]=mmio_inb(memory+(entry*32), 24);
		fat_entry[entry].lfs_name[11]=mmio_inb(memory+(entry*32), 28);
		fat_entry[entry].lfs_name[12]=mmio_inb(memory+(entry*32), 30);
		fat_entry[entry].lfs_name[13]=0;
	}
}

void fat_test(void) {
	uint8_t entry=0;

	p(fat_info.oem);
	pv(fat_table_start);
	pv(fat_data_start);

	p("FAT TABLE");
	for(int i=0; i<20; i++) {
		ph(get_fat_cluster(i));
	}

	read_fat_cluster(2, (uint32_t)fat_file);

	entry=3;
	read_fat_entry(entry, (uint32_t)fat_file);
	p(fat_entry[entry].name);
	p(fat_entry[entry].extension);
	p(fat_entry[entry].lfs_name);
}
