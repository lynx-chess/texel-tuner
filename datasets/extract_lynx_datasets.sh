#!/bin/bash

sed -n '1,7000000p;7000001q' lichess-big3-resolved.book > lichess-big3-resolved_0-7000000.texeltuner
sed -n '1,777777p;777778q' stash_gedas_formatted.epd > stash_gedas_formatted_0-777777.texeltuner
sed -n '1,777777p;777778q' gedas_filtered_sf_d9_formatted.epd > gedas_filtered_sf_d9_formatted_0-777777.texeltuner
sed -n '1,500000p;500001q' pedantic_master_training_data_ver7.0d_wdl.csv > pedantic_master_training_data_ver7.0d_wdl_0-500000.texeltuner
sed -n '1,777777p;777778q' E12.46FRC-1250k-D12-1s-Resolved.book > E12.46FRC-1250k-D12-1s-Resolved_0-7777777.texeltuner
