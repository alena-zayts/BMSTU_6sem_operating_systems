cmd_/media/psf/Home/Desktop/BMSTU_6sem_operating_systems/BMSTU_6sem_operating_systems/lab_03/part_01/Module.symvers := sed 's/\.ko$$/\.o/' /media/psf/Home/Desktop/BMSTU_6sem_operating_systems/BMSTU_6sem_operating_systems/lab_03/part_01/modules.order | scripts/mod/modpost -m -a  -o /media/psf/Home/Desktop/BMSTU_6sem_operating_systems/BMSTU_6sem_operating_systems/lab_03/part_01/Module.symvers -e -i Module.symvers   -T -