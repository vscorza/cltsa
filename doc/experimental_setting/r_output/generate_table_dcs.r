list.of.packages <- c("xtable","dplyr","magrittr","ggplot2","grid","ggthemes","gridExtra","lattice","base","plot3D")
new.packages <- list.of.packages[!(list.of.packages %in% installed.packages()[,"Package"])]
if(length(new.packages)) install.packages(new.packages)
library(base)
library(xtable)
library(magrittr) # needs to be run every time you start R and want to use %>%
library(dplyr)
library(ggplot2)
library(gridExtra)
library(lattice)
require(scales)

library(stringr)

theme_Publication <- function(base_size=14, base_family="libertine") {
  library(grid)
  library(ggthemes)
  (theme_foundation(base_size=base_size)
    + theme(plot.title = element_text(face = "bold",
                                      size = rel(1.2), hjust = 0.5),
            text = element_text(),
            panel.background = element_rect(colour = NA),
            plot.background = element_rect(colour = NA),
            panel.border = element_rect(colour = NA),
            axis.title = element_text(face = "bold",size = rel(1)),
            axis.title.y = element_text(angle=90,vjust =2),
            axis.title.x = element_text(vjust = -0.2),
            axis.text = element_text(), 
            axis.line = element_line(colour="black"),
            axis.ticks = element_line(),
            panel.grid.major = element_line(colour="#f0f0f0"),
            panel.grid.minor = element_blank(),
            legend.key = element_rect(colour = NA),
            legend.position = "bottom",
            legend.direction = "horizontal",
            legend.key.size= unit(0.2, "cm"),
            legend.margin = unit(0, "cm"),
            legend.title = element_text(face="italic"),
            plot.margin=unit(c(10,5,5,5),"mm"),
            strip.background=element_rect(colour="#f0f0f0",fill="#f0f0f0"),
            strip.text = element_text(face="bold")
    ))
  
}

scale_fill_Publication <- function(...){
  library(scales)
  discrete_scale("fill","Publication",manual_pal(values = c("#386cb0","#fdb462","#7fc97f","#ef3b2c","#662506","#a6cee3","#fb9a99","#984ea3","#ffff33")), ...)
  
}

scale_colour_Publication <- function(...){
  library(scales)
  discrete_scale("colour","Publication",manual_pal(values = c("#386cb0","#fdb462","#7fc97f","#ef3b2c","#662506","#a6cee3","#fb9a99","#984ea3","#ffff33")), ...)
  
}

experimental_composite <- read.csv(file='/home/mariano/git_repo/henos/henos-automata/doc/experimental_setting/tmp_results/unrealizable_data_composite.csv')
experimental_composite$name  <- str_replace_all(experimental_composite$name, "Scheduler", "Arbiter")
experimental_composite$name  <- str_replace_all(experimental_composite$name, "\\.\\(missing\\.assumption\\)", "\\.(missing\\.liveness)")
experimental_composite$name  <- str_replace_all(experimental_composite$name, "\\.missing\\.assumption", "\\.(missing\\.liveness)")
experimental_composite$name  <- str_replace_all(experimental_composite$name, "\\.removed\\.safety", "\\.(removed\\.safety)")
experimental_composite$name  <- str_replace_all(experimental_composite$name, "\\.", " ")
experimental_composite$liveness_count <- experimental_composite$assumptions_count + experimental_composite$guarantees_count
experimental_composite <- subset(experimental_composite, select = -c(realizable, ltl_model_build_time, model_build_time, composition_time,synthesis_time,alphabet_size,assumptions_count,guarantees_count,plant_states,plant_transitions,plant_controllable_transitions,plant_controllable_options,search_method))
#creating realizable data frame
realizable_data <- read.csv(file='/home/mariano/git_repo/henos/henos-automata/doc/experimental_setting/tmp_results/realizable_data_composite.csv')
realizable_data$name  <- str_replace_all(realizable_data$name, "Scheduler", "Arbiter")
realizable_data$name  <- str_replace_all(realizable_data$name, "\\.", " ")
experimental_composite$name  <- str_to_title(experimental_composite$name)
realizable_data$name  <- str_to_title(realizable_data$name)
realizable_data$name  <- str_replace_all(realizable_data$name, " Realizable", "")
realizable_data$ctrl_transitions <- realizable_data$minimization_transitions
realizable_data <- subset(realizable_data, select = -c(diagnosis_time,minimization_states,minimization_transitions,minimization_controllable_transitions,minimization_controllable_options,search_method,diagnosis_steps,realizable))
#merging data frames (realizable + liveness + safety)
realizable_data$liveness_count <- realizable_data$assumptions_count + realizable_data$guarantees_count
missing_assumptions <- subset(experimental_composite, grepl("Missing Liveness", name, fixed=TRUE))
missing_assumptions$name <- str_replace_all(missing_assumptions$name, " \\(Missing Liveness\\)", "")
missing_assumptions$diagnosis_time_liveness <- missing_assumptions$diagnosis_time
missing_assumptions$minimization_states_liveness <- missing_assumptions$minimization_states
missing_assumptions$minimization_transitions_liveness <- missing_assumptions$minimization_transitions
missing_assumptions$minimization_controllable_transitions_liveness <- missing_assumptions$minimization_controllable_transitions
missing_assumptions$minimization_controllable_options_liveness <- missing_assumptions$minimization_controllable_options
missing_assumptions$diagnosis_steps_liveness <- missing_assumptions$diagnosis_steps
missing_assumptions$liveness_count_liveness <- missing_assumptions$liveness_count
missing_assumptions <- subset(missing_assumptions, select = -c(diagnosis_time, minimization_states, minimization_transitions, minimization_controllable_transitions, minimization_controllable_options, diagnosis_steps, liveness_count))

removed_safety <- subset(experimental_composite, grepl("Removed Safety", name, fixed=TRUE))
removed_safety$name <- str_replace_all(removed_safety$name, " \\(Removed Safety\\)", "")
removed_safety$name <- str_replace_all(removed_safety$name, " \\(Missing Liveness\\)", "")
removed_safety$diagnosis_time_safety <- removed_safety$diagnosis_time
removed_safety$minimization_states_safety <- removed_safety$minimization_states
removed_safety$minimization_transitions_safety <- removed_safety$minimization_transitions
removed_safety$minimization_controllable_transitions_safety <- removed_safety$minimization_controllable_transitions
removed_safety$minimization_controllable_options_safety <- removed_safety$minimization_controllable_options
removed_safety$diagnosis_steps_safety <- removed_safety$diagnosis_steps
removed_safety$liveness_count_safety <- removed_safety$liveness_count
removed_safety <- subset(removed_safety, select = -c(diagnosis_time, minimization_states, minimization_transitions, minimization_controllable_transitions, minimization_controllable_options, diagnosis_steps,liveness_count))


compared_data_assumptions <- merge(realizable_data, missing_assumptions, by.x ="name", by.y ="name", sort = FALSE)
compared_data_assumptions$reduction_ratio <- compared_data_assumptions$minimization_transitions_liveness / compared_data_assumptions$plant_transitions
compared_data_assumptions$ctrl_ratio <- compared_data_assumptions$minimization_transitions_liveness / compared_data_assumptions$ctrl_transitions
compared_data_assumptions$time_ratio <- compared_data_assumptions$diagnosis_time_liveness / compared_data_assumptions$synthesis_time
summ_liveness <- compared_data_assumptions[compared_data_assumptions$plant_transitions >= 1000, ] %>%
  group_by(name) %>%
  summarize(liveness_count = max(liveness_count_liveness), synthesis_time=max(synthesis_time),diag_time = max(diagnosis_time_liveness),
            time_ratio = max(time_ratio), plant_transitions=max(plant_transitions), 
            minimization_transitions_liveness=max(minimization_transitions_liveness), 
            ctrl_transitions = max(ctrl_transitions), reduction_ratio=max(reduction_ratio),ctrl_ratio=max(ctrl_ratio))

table_contents <- xtable(summ_liveness, type = "latex", align = "r|l|r|rrr|rrr|rr|",caption="Quantitative results for minimization plants"
                         ,digits=c(0,0,2,2,2,0,0,0,2,2,0))
table_contents$synthesis_time <- with(table_contents, ifelse(synthesis_time < 1, '$<$1', round(synthesis_time)))
table_contents$diag_time <- with(table_contents, ifelse(diag_time < 1, '$<$1', round(diag_time)))
table_contents$reduction_ratio <- ifelse(table_contents$reduction_ratio < 0.00001, "$<1e^{-5}$ \\%",paste(format(round((table_contents$reduction_ratio * 100), 4), nsmall = 2), "\\%"))
table_contents$ctrl_ratio <- ifelse(table_contents$ctrl_ratio < 0.00001, "$<1e^{-5}$ \\%",paste(format(round((table_contents$ctrl_ratio * 100), 4), nsmall = 2), "\\%"))
names(table_contents) <- c('Name', '$|\\varphi_e + \\varphi_s|$', 'Synth. time(s)', 'Diag. time(s)', '$t_{\\mathcal{C}}$=Diag. time/Synth. time', 
                           '$|\\Delta_E|$', "$|\\Delta_{E'}|$", "$|\\Delta_{C}|$", "$v_{\\mathcal{U}}=|\\Delta_{E'}|/|\\Delta_{E}|$", "$v_{\\mathcal{C}}=|\\Delta_{E'}|/|\\Delta_{C}|$")
addtorow <- list()
addtorow$pos <- list(-1)
addtorow$command <- paste0(paste0('\\hline & & \\multicolumn{3}{c|}{Time}&\\multicolumn{3}{c|}{Volume} & \\multicolumn{2}{c|}{Reduction}', collapse=''), '\\\\')

print(table_contents, file = "/home/mariano/git_repo/henos/henos-automata/doc/experimental_setting/tmp_results/experimental_compared_data_liveness.tex", 
      add.to.row = addtorow, floating= FALSE, include.rownames=FALSE, sanitize.text.function=function(x){x})

compared_data_safety <- merge(realizable_data, removed_safety, by.x ="name", by.y ="name", sort = FALSE)
compared_data_safety$reduction_ratio <- compared_data_safety$minimization_transitions_safety / compared_data_safety$plant_transitions
compared_data_safety$ctrl_ratio <- compared_data_safety$minimization_transitions_safety / compared_data_safety$ctrl_transitions
compared_data_safety$time_ratio <- compared_data_safety$diagnosis_time_safety / compared_data_safety$synthesis_time
summ_safety <- compared_data_safety[compared_data_safety$plant_transitions >= 1000, ] %>%
  group_by(name) %>%
  summarize(liveness_count = max(liveness_count_safety), diag_time = max(diagnosis_time_safety),
            time_ratio = max(time_ratio), plant_transitions=max(plant_transitions), 
            minimization_transitions_safety=max(minimization_transitions_safety), 
            ctrl_transitions = max(ctrl_transitions), reduction_ratio=max(reduction_ratio),ctrl_ratio=max(ctrl_ratio))

table_contents <- xtable(summ_safety, type = "latex", align = "l|r|rrr|rrr|rr|",caption="Quantitative results for minimization plants"
                         ,digits=c(0,0,2,2,2,0,0,0,2,2))
table_contents$synthesis_time <- with(table_contents, ifelse(synthesis_time < 1, '$<$1', round(synthesis_time)))
table_contents$diag_time <- with(table_contents, ifelse(diag_time < 1, '$<$1', round(diag_time)))
table_contents$reduction_ratio <- ifelse(table_contents$reduction_ratio < 0.00001, "$<1e^{-5}$ \\%",paste(format(round((table_contents$reduction_ratio * 100), 4), nsmall = 2), "\\%"))
table_contents$ctrl_ratio <- ifelse(table_contents$ctrl_ratio < 0.00001, "$<1e^{-5}$ \\%",paste(format(round((table_contents$ctrl_ratio * 100), 4), nsmall = 2), "\\%"))
names(table_contents) <- c('Name', '$|\\varphi_e + \\varphi_s|$', 'Synth. time(s)', 'Diag. time(s)', '$t_{\\mathcal{C}}$=Diag. time/Synth. time', 
                           '$|\\Delta_E|$', "$|\\Delta_{E'}|$", "$|\\Delta_{C}|$", "$v_{\\mathcal{U}}=|\\Delta_{E'}|/|\\Delta_{E}|$", "$v_{\\mathcal{C}}=|\\Delta_{E'}|/|\\Delta_{C}|$")
addtorow <- list()
addtorow$pos <- list(-1)
addtorow$command <- paste0(paste0('\\hline  & & \\multicolumn{3}{c|}{Time}&\\multicolumn{3}{c|}{Volume} & \\multicolumn{2}{c|}{Reduction}', collapse=''), '\\\\')

print(table_contents, file = "/home/mariano/git_repo/henos/henos-automata/doc/experimental_setting/tmp_results/experimental_compared_data_safety.tex", 
      add.to.row = addtorow, floating= FALSE, include.rownames=FALSE, sanitize.text.function=function(x){x})

