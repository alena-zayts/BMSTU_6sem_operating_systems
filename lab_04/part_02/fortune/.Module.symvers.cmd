cmd_/media/psf/Home/Desktop/BMSTU_6sem_operating_systems/lab_04/part_02/fortune/Module.symvers := sed 's/\.ko$$/\.o/' /media/psf/Home/Desktop/BMSTU_6sem_operating_systems/lab_04/part_02/fortune/modules.order | scripts/mod/modpost -m -a  -o /media/psf/Home/Desktop/BMSTU_6sem_operating_systems/lab_04/part_02/fortune/Module.symvers -e -i Module.symvers   -T -