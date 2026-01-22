if if echo oui
then
    true;
else
    false;
fi; then 
    echo "it work!!!"
else
    echo "Crap!"
fi

if if echo oui
then
    false;
else
    false;
fi; then 
    echo "don't work!!!"
elif if false;
then false;
elif echo "pls"
then true;
fi
then
    echo "work again";
else
    echo "Crap!"
fi
