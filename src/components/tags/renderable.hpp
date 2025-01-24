namespace Tags
{
    struct Renderable
    {
        int z = 0;

        /* TODO: deprecate as soon as EnTT
         * implements enabled/disabled components */
        bool render = true;
    };
}
