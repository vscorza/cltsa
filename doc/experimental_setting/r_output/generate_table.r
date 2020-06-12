list.of.packages <- c("xtable","dplyr","magrittr")
new.packages <- list.of.packages[!(list.of.packages %in% installed.packages()[,"Package"])]
if(length(new.packages)) install.packages(new.packages)
library(xtable)
library(magrittr) # needs to be run every time you start R and want to use %>%
library(dplyr)
experimental_composite <- read.csv(file='/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/unrealizable_data_composite.csv')
experimental_composite$liveness_count <- experimental_composite$guarantees_count + experimental_composite$assumptions_count
experimental_composite$reduction <- experimental_composite$minimization_transitions / experimental_composite$plant_transitions
summ_unreal <- experimental_composite %>%
  group_by(name) %>%
  summarize(diag = mean(diagnosis_time), liveness = max(liveness_count), plant_trans = max(plant_transitions),
            min_trans = max(minimization_transitions), red = mean(reduction), steps = max(diagnosis_steps))
realizable_composite = read.csv(file='/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/realizable_data_composite.csv')
summ_real <- realizable_composite %>%
  group_by(name) %>%
  summarize(plant_states = max(plant_states), plant_trans = max(plant_transitions),  plant_trans = max(plant_transitions),  min_trans = max(minimization_transitions))

genbuf_missing <- subset(summ_unreal, grepl("Genbuf\\.\\d\\.\\(missing", name))
genbuf_removed <- subset(summ_unreal, grepl("Genbuf\\.\\d\\.\\(removed", name))
collector_missing <- subset(summ_unreal, grepl("Collector\\.\\d\\.\\(missing", name))
robot_samples <- subset(summ_unreal, grepl("Robot", name))

genbuf_real <- subset(summ_real, grepl("Genbuf", name))
collector_real <- subset(summ_real, grepl("Collector", name))
robot_real <- subset(summ_real, grepl("Robot", name))

genbuf_missing$reduction_ctrl <- genbuf_missing$plant_trans / genbuf_real$plant_trans
genbuf_missing$ctrl_states <- genbuf_real$plant_states
genbuf_missing$ctrl_transitions <- genbuf_real$plant_trans
genbuf_removed$reduction_ctrl <- genbuf_removed$plant_trans / genbuf_real$plant_trans
genbuf_removed$ctrl_states <- genbuf_real$plant_states
genbuf_removed$ctrl_transitions <- genbuf_real$plant_trans
collector_missing$reduction_ctrl <- collector_missing$plant_trans / collector_real$plant_trans
collector_missing$ctrl_states <- collector_real$plant_states
collector_missing$ctrl_transitions <- collector_real$plant_trans
robot_samples$reduction_ctrl <- robot_samples$plant_trans / robot_real$plant_trans
robot_samples$ctrl_states <- robot_real$plant_states
robot_samples$ctrl_transitions <- robot_real$plant_trans

composite_table <- rbind(collector_missing, robot_samples, genbuf_missing, genbuf_removed)

print(xtable(composite_table, type = "latex"), file = "/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/experimental_data.tex")
